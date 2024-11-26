from fastapi import FastAPI, HTTPException
from contextlib import asynccontextmanager
import subprocess
import threading
import logging

# Configurazione logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

app = FastAPI()

# Variabili globali
proc = None
proc_lock = threading.Lock()

@asynccontextmanager
async def lifespan(app: FastAPI):
    global proc
    try:
        # Avvia il processo
        logger.info("Avvio del processo...")
        proc = subprocess.Popen(
            ["./anagrammer_listener", "dictionary/wordlist95000_ITA_sign.json"],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            bufsize=1
        )
        logger.info("Processo avviato con successo.")
        yield
    except Exception as e:
        logger.error(f"Errore durante l'avvio del processo: {e}")
        raise RuntimeError(f"Errore durante l'avvio: {e}")
    finally:
        if proc:
            logger.info("Terminazione del processo...")
            proc.terminate()
            proc = None

app = FastAPI(lifespan=lifespan)

@app.get("/anagram")
def get_anagram(query: str):
    global proc, proc_lock
    if not proc:
        raise HTTPException(status_code=500, detail="Il processo anagrammer non è in esecuzione")

    with proc_lock:
        try:
            # Invio query al processo
            proc.stdin.write(query + "\n")
            proc.stdin.flush()

            # Leggo la risposta
            anagrams = []
            while True:
                line = proc.stdout.readline().strip()
                if line == "END_OF_RESULT":
                    break
                anagrams.append(line)

            return {"anagrams": anagrams}

        except Exception as e:
            logger.error(f"Errore nella comunicazione col processo: {e}")
            raise HTTPException(status_code=500, detail=str(e))

# Utilizza un'immagine base di Python
FROM python:3.9-slim

# Aggiorna il sistema e installa g++ per compilare il codice C++
RUN apt-get update && apt-get install -y \
    g++ \
    && rm -rf /var/lib/apt/lists/*

# Imposta la directory di lavoro
WORKDIR /app

# Copia il file del dizionario nel container
COPY dictionary /app/dictionary

# Copia il codice sorgente C++
COPY anagrammer_listener.cpp /app

# Compila l'eseguibile C++
RUN g++ -O3 -o anagrammer_listener anagrammer_listener.cpp

# Rendi l'eseguibile eseguibile
RUN chmod +x /app/anagrammer_listener

# Copia il codice Python e il file requirements.txt
COPY main.py /app
COPY requirements.txt /app

# Installa le dipendenze Python
RUN pip install --no-cache-dir -r requirements.txt

# Espone la porta 8000 per il server FastAPI
EXPOSE 8051

# Comando per avviare il server all'avvio del container
CMD ["uvicorn", "main:app", "--host", "0.0.0.0", "--port", "80051"]

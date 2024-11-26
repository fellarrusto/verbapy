# Base Python image
FROM python:3.9-slim-buster

# Update and install dependencies
RUN apt-get update && apt-get install -y \
    g++ \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /app

# Copy dictionary and source files into the container
COPY dictionary /app/dictionary
COPY src/anagrammer_listener.cpp /app
COPY src/include/anagrammer.h /app

# Compile the C++ executable
RUN g++ -Wall -Wextra -std=c++17 -O3 -o anagrammer_listener anagrammer_listener.cpp

# Copy the Python application and requirements
COPY run_background_listener.py /app
COPY requirements.txt /app

# Install Python dependencies
RUN pip install --no-cache-dir -r requirements.txt

# Expose the port for FastAPI
EXPOSE 8051

# Command to start the FastAPI server
CMD ["uvicorn", "run_background_listener:app", "--host", "0.0.0.0", "--port", "8051"]
import tkinter as tk
from tkinter import Text
from threading import Thread
import subprocess
import os
import signal

client_process = None
running = True  # Indicateur pour contrôler l'exécution du thread

def connect():
    global client_process, receive_thread, running
    username = nom_entry.get().strip()
    hostname = hostname_entry.get().strip()
    
    client_process = subprocess.Popen(["../version_text/./client"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    client_process.stdin.write((username + "\n"))
    client_process.stdin.flush()
    client_process.stdin.write((hostname + "\n"))
    client_process.stdin.flush()

    line = client_process.stdout.readline().strip()
    if "Connecté au serveur." in line:
        # La connexion est bien établie        
        login_frame.pack_forget()  # Hide login interface
        chat_frame.pack()  # Show chat interface
        
        receive_thread = Thread(target=receive_message)
        receive_thread.start()
        
    else:
        popup_window("Host introuvable ou probleme lors de la connexion.")

#creation de la fenetre d'erreur de connexion
def popup_window(message):
    popup = tk.Toplevel()
    popup.title("Error")
    label = tk.Label(popup, text=message, fg="red")
    label.pack()
    popup.after(3000, popup.destroy)

# fonction pour l'envoi des entrée clavier au serveur 
def send_message():
    message = input_entry.get()
    input_entry.delete(0, tk.END)
    client_process.stdin.write(message + '\n')
    client_process.stdin.flush()
    display_message(f"Vous : {message}")  # Afficher le message envoyé dans l'interface de discussion

    # Ne pas afficher le message envoyé car le widget Text est en lecture seule

# fonction pour lire et afficher les messages
def receive_message():
    while running:
        line = client_process.stdout.readline().strip()
        if line:
            display_message(line)

# affichage des messages 
def display_message(message):
    chat_text.config(state=tk.NORMAL)  # Autoriser l'édition temporairement
    chat_text.insert(tk.END, f"{message}\n")
    chat_text.config(state=tk.DISABLED)  # Rendre le widget en lecture seule
    chat_text.see(tk.END)

# deconnection ou quitter l'interface graphique
def exit_app():
    global running
    running = False  # Met à False l'indicateur pour arrêter le thread
    if client_process:
        os.kill(client_process.pid, signal.SIGINT)
    root.destroy()



root = tk.Tk()
root.title("Client")

# Login interface
login_frame = tk.Frame(root)
login_frame.pack()

nom_label = tk.Label(login_frame, text="Enter your name:")
nom_label.pack()

nom_entry = tk.Entry(login_frame, width=30)
nom_entry.pack()

hostname_label = tk.Label(login_frame, text="Enter the server hostname:")
hostname_label.pack()

hostname_entry = tk.Entry(login_frame, width=30)
hostname_entry.pack()

connect_button = tk.Button(login_frame, text="Connect", command=connect)
connect_button.pack()

# Chat interface
chat_frame = tk.Frame(root)

chat_text = Text(chat_frame, width=80, height=40, state=tk.DISABLED)  # widget Text en lecture seule
chat_text.pack()

input_entry = tk.Entry(chat_frame, width=50)
input_entry.pack()

send_button = tk.Button(chat_frame, text="Send", command=send_message)
send_button.pack()

exit_button = tk.Button(chat_frame, text="Exit", command=exit_app)
exit_button.pack()

root.mainloop()

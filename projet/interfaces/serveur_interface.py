import tkinter as tk
from threading import Thread
import subprocess
import queue

class ServerApp:
    def __init__(self, master):
        self.master = master
        master.title("Server Interface")

        self.output_text = tk.Text(master, height=20, width=80)
        self.output_text.pack()
        self.output_text.bind("<Key>", lambda e: "break")  # Empêche la saisie utilisateur
        self.start_button = tk.Button(master, text="Start Server", command=self.start_server)
        self.start_button.pack()

        self.stop_button = tk.Button(master, text="Stop Server", command=self.stop_server, state=tk.DISABLED)
        self.stop_button.pack()

        self.server_started = False
        self.process = None
        self.stdout_queue = queue.Queue()
        

    def start_server(self):
        if not self.server_started:
            self.process = subprocess.Popen(["../version_text/./server"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
            self.server_started = True

            self.start_button.config(state=tk.DISABLED)
            self.stop_button.config(state=tk.NORMAL)

             # Démarrage de deux threads pour lire à la fois la sortie standard et la sortie d'erreur du processus
            self.read_stdout_thread = Thread(target=self.read_stdout)
            self.read_stdout_thread.daemon = True
            self.read_stdout_thread.start()

            self.read_stderr_thread = Thread(target=self.read_stderr)
            self.read_stderr_thread.daemon = True
            self.read_stderr_thread.start()

            self.master.after(100, self.update_output_text)

    def stop_server(self):
        if self.process:
            self.process.terminate()
            self.output_text.insert(tk.END, "Server arreté merci aurevoir.\n")
            self.start_button.config(state=tk.NORMAL)
            self.stop_button.config(state=tk.DISABLED)

    def read_stdout(self):
        for line in iter(self.process.stdout.readline, ''):
            self.stdout_queue.put(line.strip())

    def read_stderr(self):
        # Lecture de la sortie d'erreur du processus du serveur en continu
        for line in iter(self.process.stderr.readline, ''):
            self.stdout_queue.put(line.strip())


    def update_output_text(self):
        while not self.stdout_queue.empty():
            output_line = self.stdout_queue.get()
            self.output_text.insert(tk.END, output_line + '\n')
            self.output_text.see(tk.END)
        self.master.after(100, self.update_output_text)

root = tk.Tk()
app = ServerApp(root)
root.mainloop()

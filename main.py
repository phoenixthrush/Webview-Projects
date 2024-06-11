import webview
import os
import sys

class API:
    def generate(self):
        file_path = window.create_file_dialog(webview.SAVE_DIALOG, directory=os.path.join(os.path.expanduser('~'), 'Desktop'), save_filename='Authorize.auz')
        with open(file_path, "w") as file:
            file.write("Hello, World!")

    def destroy(self):
        window.destroy()
        sys.exit()

if __name__ == '__main__':
    with open(os.path.join(os.path.dirname(__file__), "index.html"), 'r') as file:
        html = file.read()

    api = API()
    window = webview.create_window('phoenixthrush', html=html, js_api=api, width=350, height=500, frameless=True, resizable=False, shadow=True)
    webview.start()

import webview

def save_file_dialog(window):
    import time

    time.sleep(5)
    result = window.create_file_dialog(
        webview.SAVE_DIALOG, directory='/', save_filename='test.file'
    )
    print(result)

if __name__ == '__main__':
    with open('site/index.html', 'r') as file:
        html = file.read()

    window = webview.create_window('phoenixthrush', html=html, width=350, height=500, frameless=True)
    webview.start()
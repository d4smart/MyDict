from flask import Flask
from flask import request
from flask import render_template
from Mydict import *

app = Flask(__name__)

@app.route('/', methods=['GET', 'POST'])
def index():
    if request.method == 'POST':
	word = request.form['word']
	if word == None:
	    error = 'Word not get!'
	    return render_template('index.html', error=error)
	L = find(word)
	if not L:
	    error = 'Word not found!'
	    return render_template('index.html', error=error)
	types = L[0].decode('utf-8')
	sents = L[1].decode('utf-8')
        return render_template('index.html', types=types, sents=sents)
    else:
	return render_template('index.html')

if __name__ == '__main__':
    createdict()
    app.debug = True
    app.run(host='0.0.0.0')


import os, sys, json, logging, sqlite3
from flask import Flask, request, jsonify
from flask import flash, redirect, url_for
from werkzeug.utils import secure_filename
from flask import send_from_directory

UPLOAD_FOLDER = './files'
ALLOWED_EXTENSIONS = {'gif'}
DB = 'server.db3'
CREATE_SQL = '''
CREATE TABLE captures(
   ts number(20),
   gif BLOB,
   started_ts number(20),
   ended_ts number(20),
   frames_qty number(10),
   monitor_id number(4)
 )
'''

app = Flask(__name__)
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER

if os.path.exists(DB):
	os.remove(DB)

if not os.path.exists(DB):
	con = sqlite3.connect(DB)
	cur = con.cursor()
	cur.execute(CREATE_SQL)
	con.commit()    
	cur.close()	

def insert_capture(ts=0, gif='', started_ts=0, ended_ts=0, frames_qty=0, monitor_id=0):
    data = (ts,gif,started_ts,ended_ts,frames_qty,monitor_id)
    con = sqlite3.connect(DB)
    cur = con.cursor()
    cur.execute("""INSERT INTO captures 
		(ts, gif, started_ts, ended_ts, frames_qty, monitor_id)
		VALUES
		(?, ?, ?, ?, ?, ?)""", data)
    con.commit()
    cur.close()
    return cur.lastrowid

def allowed_file(filename):
    return '.' in filename and \
           filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS

@app.route('/uploads/<name>')
def download_file(name):
    return send_from_directory(app.config["UPLOAD_FOLDER"], name)

@app.route('/capture', methods=['POST'])
def capture():
    data = [
	(12345,'xxxxxxxxxx',2222,3333,12,0)
    ]
    req = request.json
    inserted_id = insert_capture(
	  ts=req['ts'],
	  gif=req['gif'],
	  started_ts=req['started_ts'],
	  ended_ts=req['ended_ts'],
	  frames_qty=req['frames_qty'],
	  monitor_id=req['monitor_id'],
    )
    resp = {
	'success': True,
	'id': inserted_id,
    }

    return jsonify(resp)


@app.route('/data', methods=['POST']) 
def foo():
    data = request.json
    j = jsonify(data)
    logging.debug(f'req: {data}')
    print(j)
    return j


@app.route('/file', methods=['GET', 'POST'])
def upload_file():
    if request.method == 'POST':
        print(request.files)
        if 'file' not in request.files:
            flash('No file part')
            return redirect(request.url)
        file = request.files['file']
        if file.filename == '':
            flash('No selected file')
            return redirect(request.url)
        if file and allowed_file(file.filename):
            filename = secure_filename(file.filename)
            file.save(os.path.join(app.config['UPLOAD_FOLDER'], filename))
            return redirect(url_for('download_file', name=filename))
    return '''
    <!doctype html>
    <title>Upload new File</title>
    <h1>Upload new File</h1>
    <form method=post enctype=multipart/form-data>
      <input type=file name=file>
      <input type=submit value=Upload>
    </form>
    '''

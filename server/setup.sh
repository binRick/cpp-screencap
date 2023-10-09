[[ -d .venv ]] || python3 -m venv .venv
source .venv/bin/activate
pip3 -q install -r requirements.txt

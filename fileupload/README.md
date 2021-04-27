A demo for uploading files from the web with a progress bar using JavaScript.

![Screenshot of demo](/gh_images/screenshot1.png)
![Screenshot of demo](/gh_images/screenshot2.png)

Try it yourself:

```shell
# Clone the code.
git clone https://github.com/iafisher/demos.git
cd demos/fileupload

# Set up a virtual environment for Python dependencies.
# Optional but recommended.
virtualenv --python=python3 .venv
.venv/bin/activate

# Install the Python dependencies.
pip3 install -r requirements.txt

# Start the server.
python3 server.py
```

Then, visit http://localhost:5000 in your browser of choice to view the demo.

Note that even large uploads are likely to be very quick when the server is running locally, so you may wish to open up Chrome's developer tools and artifically throttle the connection under the "Network" tab so that you can see the progress bar updating.

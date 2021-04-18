import base64

from flask import Flask, render_template, request

app = Flask(__name__)


@app.route("/")
def home():
    return render_template("index.html")


@app.route("/api/files/upload", methods=["POST"])
def api_files_upload():
    payload = request.get_json()
    # `contents` is encoded as a data URL:
    #   https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Data_URIs
    #
    # The 'data:*/*;base64,' prefix must be removed before the base64 contents can be
    # decoded.
    contents = payload["contents"]
    if not contents.startswith("data:"):
        return (
            {
                "error": "Expected `contents` field to be a data URL starting with "
                + "'data:*/*;base64,'"
            },
            400,
        )

    marker = ";base64,"
    index = contents.find(marker)
    contents = base64.b64decode(contents[index + len(marker) :])
    # Now `contents` is a bytes object containing the actual bytes of the file that the
    # user uploaded.
    handle_file_upload(payload["name"], contents)
    return {"success": True}


def handle_file_upload(name, contents):
    pass


if __name__ == "__main__":
    app.run(debug=True)

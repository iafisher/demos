/**
 * If you are reading this file to figure out how to use this technique
 * yourself, you only need to look at `main` and `uploadFile`. The other
 * functions are just for this demo's UI.
 */


function main() {
  const input = document.getElementById("file-input");
  const button = document.getElementById("upload-button");

  // A trick to use a custom button instead of the browser's default, ugly
  // widget for <input type="file"> elements. The idea is to hide the <input>
  // (see static/index.css) and then pass on all click events from our custom
  // button to the hidden input.
  //
  // Courtesy of https://developer.mozilla.org/en-US/docs/Web/API/File/Using_files_from_web_applications#using_hidden_file_input_elements_using_the_click_method
  button.addEventListener("click", () => {
    input.click();
  });

  // Start uploading files once they are selected by the file input widget.
  input.addEventListener("change", (event) => {
    const files = event.target.files;
    for (const file of files) {
      uploadFile(file);
    }
  });
}


/**
 * Sends an AJAX request to upload the file, and update the progress bar as the
 * file is uploaded.
 */
function uploadFile(file) {
  // We use the XMLHttpRequest API instead of the newer Fetch API because the
  // latter does not support the progress listener.
  const xhr = new XMLHttpRequest();

  // Create an HTML node to display the upload's status.
  const node = prependEntry(file.name);

  // HTML5 has a native <progress> element that we use here.
  //
  // Courtesy of https://www.sitepoint.com/html5-javascript-file-upload-progress-bar/
  // More information:
  //   https://developer.mozilla.org/en-US/docs/Web/API/XMLHttpRequest/Using_XMLHttpRequest#monitoring_progress
  const progress = node.querySelector("progress");

  // Set up a listener for when the upload finishes.
  //
  // More information:
  //   https://developer.mozilla.org/en-US/docs/Web/API/XMLHttpRequest/onreadystatechange
  xhr.onreadystatechange = (event) => {
    if (xhr.readyState !== XMLHttpRequest.DONE) {
      return;
    }

    // Take the appropriate action depending on whether the AJAX request
    // succeeded or failed.
    if (xhr.status >= 200 && xhr.status < 300) {
      markNodeAsFinished(node);
    } else {
      // For HTTP 400 errors, the response will be a JSON object with an
      // `error` field.
      if (xhr.status >= 400 && xhr.status < 500) {
        console.error(JSON.parse(xhr.responseText).error);
      }
      markNodeAsFailed(node, "Error while uploading file.");
    }
  };

  // Whenever a new progress event fires, update the <progress> element.
  xhr.upload.addEventListener("progress", (event) => {
    progress.max = event.total;
    progress.value = event.loaded;
  });

  // Open the AJAX request.
  xhr.open("POST", "/api/files/upload", true);
  xhr.setRequestHeader("Content-Type", "application/json");

  // Encode the file contents, which we treat as binary data, as a base64
  // string using the FileReader API, since JSON does not directly support
  // binary data.
  //
  // Courtesy of https://stackoverflow.com/questions/18650168
  // More information: https://developer.mozilla.org/en-US/docs/Web/API/FileReader
  const reader = new FileReader();
  // Set up a callback to send the AJAX request when base64 conversion
  // finishes.
  reader.onloadend = () => {
    xhr.send(JSON.stringify({ name: file.name, contents: reader.result }));
  };
  reader.readAsDataURL(file);
}


/**
 * Creates a new HTML node to display the upload's status and prepends it to
 * the list of active and finished uploads.
 */
function prependEntry(name) {
  const container = document.getElementById("requests");

  const node = E("li", [
    E("div", [
      E("span", name, { classList: ["file-name"] }),
      // More information on <progress>:
      //   https://developer.mozilla.org/en-US/docs/Web/HTML/Element/progress
      E("progress", [], { attrs: { value: 0 } }),
    ]),
    E("div", [E("span", new Date().toString(), { classList: ["timestamp"] })]),
  ]);

  container.insertBefore(node, container.firstChild);
  return node;
}


function markNodeAsFinished(node) {
  replaceProgressBarWithMessage(node, "Finished!", "finished");
}


function markNodeAsFailed(node, message) {
  replaceProgressBarWithMessage(node, "Upload failed.", "failed");
}


function replaceProgressBarWithMessage(node, message, cls) {
  node
    .querySelector("progress")
    .replaceWith(E("span", message, { classList: ["message", cls] }));
}


/**
 * Helper method to construct an HTML element.
 *
 * `options` is an optional objects containing two keys, both also optional:
 *
 *   `classList`: a list of CSS classes to set on the element
 *   `attrs`: a list of HTML attributes to set on the element
 */
function E(tag, textOrChildren, options) {
  const node = document.createElement(tag);
  // Courtesy of https://stackoverflow.com/questions/4059147
  if (typeof textOrChildren === "string" || textOrChildren instanceof String) {
    node.textContent = textOrChildren;
  } else if (textOrChildren) {
    for (const child of textOrChildren) {
      node.appendChild(child);
    }
  }

  if (options) {
    if (options.classList) {
      for (const cls of options.classList) {
        node.classList.add(cls);
      }
    }

    if (options.attrs) {
      console.log(options.attrs);
      for (const [key, value] of Object.entries(options.attrs)) {
        node.setAttribute(key, value);
      }
    }
  }

  return node;
}


document.addEventListener("DOMContentLoaded", main);

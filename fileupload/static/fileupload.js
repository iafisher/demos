function main() {
  const input = document.getElementById("file-input");
  const button = document.getElementById("upload-button");

  // Courtesy of https://developer.mozilla.org/en-US/docs/Web/API/File/Using_files_from_web_applications#using_hidden_file_input_elements_using_the_click_method
  button.addEventListener("click", () => {
    input.click();
  });

  input.addEventListener("change", (event) => {
    const files = event.target.files;
    for (const file of files) {
      uploadFile(file);
    }
  });
}

function uploadFile(file) {
  const xhr = new XMLHttpRequest();

  // More information:
  //   https://developer.mozilla.org/en-US/docs/Web/API/XMLHttpRequest/onreadystatechange
  xhr.onreadystatechange = (event) => {
    if (xhr.readyState !== XMLHttpRequest.DONE) {
      return;
    }

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

  // Courtesy of https://www.sitepoint.com/html5-javascript-file-upload-progress-bar/
  // More information:
  //   https://developer.mozilla.org/en-US/docs/Web/API/XMLHttpRequest/Using_XMLHttpRequest#monitoring_progress
  const node = prependEntry(file.name);
  const progress = node.querySelector("progress");
  xhr.upload.addEventListener("progress", (event) => {
    progress.max = event.total;
    progress.value = event.loaded;
  });

  xhr.open("POST", "/api/files/upload", true);
  xhr.setRequestHeader("Content-Type", "application/json");

  // Courtesy of https://stackoverflow.com/questions/18650168
  // More information: https://developer.mozilla.org/en-US/docs/Web/API/FileReader
  const reader = new FileReader();
  reader.onloadend = () => {
    xhr.send(JSON.stringify({ name: file.name, contents: reader.result }));
  };
  reader.readAsDataURL(file);
}

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

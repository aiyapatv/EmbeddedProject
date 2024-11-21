import { changePlantType, changePlantName } from "./backend.js";

const closeBtn = document.querySelector(".close-btn");

closeBtn.addEventListener("click", () => {
  popup.style.display = "none";
});

window.captureLeafImage = function (event) {
  const input = event.target;
  const capturedImage = document.getElementById("leafPicture");

  if (input.files && input.files[0]) {
    const reader = new FileReader();

    // Read the image file as a data URL
    reader.onload = function (e) {
      const imageData = e.target.result;
      const popup = document.getElementById("popup");
      // Display the uploaded image
      capturedImage.src = imageData;
      capturedImage.style.display = "block";
      popup.style.display = "flex";
      // Process the image using TensorFlow.js or another function
      processLeafHealth(imageData);
    };

    reader.readAsDataURL(input.files[0]); // Trigger file reading
  }
};

async function processLeafHealth(imageData) {
  const model = await tf.loadLayersModel("/model/model_architecture.json");

  // Preprocess the image
  const img = document.createElement("img");
  img.src = imageData;
  img.onload = async function () {
    // Resize image to match model input size (1024x38 or whatever is correct)
    const tensor = tf.browser
      .fromPixels(img)
      .resizeNearestNeighbor([224, 224]) // Resize to the expected input size
      .toFloat()
      .expandDims(0);

    // Make prediction
    const predictions = await model.predict(tensor).data();
    const mostPredict = Math.max(...predictions);
    const predictedClass = predictions.indexOf(mostPredict); // Get the index of the highest probability

    // Display the result
    const classNames = {
      0: "Apple___Apple_scab",
      1: "Apple___Black_rot",
      2: "Apple___Cedar_apple_rust",
      3: "Apple___healthy",
      4: "Blueberry___healthy",
      5: "Cherry_(including_sour)___Powdery_mildew",
      6: "Cherry_(including_sour)___healthy",
      7: "Corn_(maize)___Cercospora_leaf_spot Gray_leaf_spot",
      8: "Corn_(maize)___Common_rust_",
      9: "Corn_(maize)___Northern_Leaf_Blight",
      10: "Corn_(maize)___healthy",
      11: "Grape___Black_rot",
      12: "Grape___Esca_(Black_Measles)",
      13: "Grape___Leaf_blight_(Isariopsis_Leaf_Spot)",
      14: "Grape___healthy",
      15: "Orange___Haunglongbing_(Citrus_greening)",
      16: "Peach___Bacterial_spot",
      17: "Peach___healthy",
      18: "Pepper_bell___Bacterial_spot",
      19: "Pepper_bell___healthy",
      20: "Potato___Early_blight",
      21: "Potato___Late_blight",
      22: "Potato___healthy",
      23: "Raspberry___healthy",
      24: "Soybean___healthy",
      25: "Squash___Powdery_mildew",
      26: "Strawberry___Leaf_scorch",
      27: "Strawberry___healthy",
      28: "Tomato___Bacterial_spot",
      29: "Tomato___Early_blight",
      30: "Tomato___Late_blight",
      31: "Tomato___Leaf_Mold",
      32: "Tomato___Septoria_leaf_spot",
      33: "Tomato___Spider_mites Two-spotted_spider_mite",
      34: "Tomato___Target_Spot",
      35: "Tomato___Tomato_Yellow_Leaf_Curl_Virus",
      36: "Tomato___Tomato_mosaic_virus",
      37: "Tomato___healthy",
    }; // Update based on your model's classes
    document.getElementById(
      "leafState"
    ).innerText = `Leaf Health: ${classNames[predictedClass]}`;
    document.getElementById("confidence").innerText = `CONFIDENCE: ${Math.round(
      mostPredict * 100
    )}%`;
  };
}
export function togglePlantType() {
  const heading = document.getElementById("plantTypeHeading");
  const button = document.getElementById("changeType");
  button.classList.remove("rotating");

  // Use a slight delay to re-add the class
  setTimeout(() => {
    button.classList.add("rotating");
  }, 10); // 10ms delay ensures the DOM updates
  if (heading.innerText === "Decoration") {
    changePlantType(true);
  } else {
    changePlantType(false);
  }
}
export function toggleEdit() {
  const plantName = document.getElementById("plantName");
  const button = document.getElementById("editButton");

  if (button.innerHTML.includes("fa-pen-to-square")) {
    const input = document.createElement("input");
    input.type = "text";
    input.value = plantName.innerText;
    plantName.innerHTML = "";
    plantName.appendChild(input);
    button.innerHTML = "Done";
    input.addEventListener("keydown", (event) => {
      if (event.key === "Enter") {
        finishEdit(input);
      }
    });
  } else {
    const input = plantName.querySelector("input");
    finishEdit(input);
  }
}

function finishEdit(input) {
  const plantName = document.getElementById("plantName");
  const button = document.getElementById("editButton");

  // Update the plant name
  changePlantName(input.value);
  plantName.innerHTML = input.value;

  // Change the button text back to "Edit"
  button.innerHTML = `<i class="fa-regular fa-pen-to-square"></i>`;
}

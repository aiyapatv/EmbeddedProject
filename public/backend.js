// Import Firebase functions
import { initializeApp } from "https://www.gstatic.com/firebasejs/9.17.1/firebase-app.js";
import {
  getDatabase,
  ref,
  set,
  onValue,
} from "https://www.gstatic.com/firebasejs/9.17.1/firebase-database.js";

// Your web app's Firebase configuration
const firebaseConfig = {
  apiKey: "AIzaSyC_IzVVLuh6hax0F77wZd4J2z9mJV4HaRs",
  authDomain: "iotlab9-874f2.firebaseapp.com",
  databaseURL:
    "https://iotlab9-874f2-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "iotlab9-874f2",
  storageBucket: "iotlab9-874f2.appspot.com",
  messagingSenderId: "719142350411",
  appId: "1:719142350411:web:84216b0715586cda98e7ce",
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const db = getDatabase(app);

const statusRef = ref(db, "data/lightStatus"); // Firebase path to monitor
onValue(statusRef, (snapshot) => {
  const status = snapshot.val();
  const button = document.getElementById("toggleButton");
  button.innerText = `${status}`;
});

const plantType = ref(db, "data/plantType");
onValue(plantType, (snapshot) => {
  const type = snapshot.val();
  const heading = document.getElementById("plantTypeHeading");
  const image = document.getElementById("plantImage");
  heading.innerText = `${type}`;
  image.src = `/image/${type}.jpg`;
});

const plantName = ref(db, "data/plantName");
onValue(plantName, (snapshot) => {
  const name = snapshot.val();
  const heading = document.getElementById("plantName");
  heading.innerText = `${name}`;
});

const temperature = ref(db, "data/temperature");
onValue(temperature, (snapshot) => {
  const temp = snapshot.val();
  const heading = document.getElementById("temperature");
  heading.innerText = `${temp} ℃`;
});

const humidity = ref(db, "data/humidity");
onValue(humidity, (snapshot) => {
  const humidity = snapshot.val();
  const heading = document.getElementById("humidity");
  heading.innerText = `${humidity} %`;
});

const moisture = ref(db, "data/moisture");
onValue(moisture, (snapshot) => {
  const moisture = snapshot.val();
  const heading = document.getElementById("moisture");
  heading.innerText = `${moisture} %`;
});

window.toggleLight = function () {
  const button = document.getElementById("toggleButton");

  // Check current button text to decide the action
  if (button.innerText === "On") {
    set(ref(db, "data/lightStatus"), "Off")
      .then(() => {
        console.log("Command sent to Firebase: On");
      })
      .catch((error) => {
        console.error("Error sending command to Firebase", error);
      });
  } else {
    set(ref(db, "data/lightStatus"), "On")
      .then(() => {
        console.log("Command sent to Firebase: Off");
      })
      .catch((error) => {
        console.error("Error sending command to Firebase", error);
      });
  }
};

export async function changePlantType(value) {
  if (value) {
    set(ref(db, "data/plantType"), "Vegetable");
  } else {
    set(ref(db, "data/plantType"), "Decoration");
  }
}

export async function changePlantName(value) {
    set(ref(db, "data/plantName"), value);
}

// let recognition; // Declare recognition variable

// Function to start voice recognition
// window.startVoiceRecognition = function () {
//   if (!recognition) {
//     recognition = new (window.SpeechRecognition ||
//       window.webkitSpeechRecognition)();

//     recognition.onresult = (event) => {
//       const command = event.results[0][0].transcript;
//       document.getElementById(
//         "command"
//       ).innerText = `Command: ${command}`;

//       // Send recognized command to Firebase Realtime Database
//       set(ref(db, "voiceCommand"), command)
//         .then(() => {
//           console.log("Command sent to Firebase");
//         })
//         .catch((error) => {
//           console.error("Error sending command to Firebase", error);
//         });
//     };

//     recognition.onerror = (event) => {
//       console.error("Speech recognition error", event.error);
//     };
//   }

//   recognition.start();
//   console.log("Voice recognition started");
// };

// // Function to stop voice recognition
// window.stopVoiceRecognition = function () {
//   if (recognition) {
//     recognition.stop();
//     console.log("Voice recognition stopped");
//   }
// };
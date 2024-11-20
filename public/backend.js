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

const statusRef = ref(db, "lightStatus"); // Firebase path to monitor
onValue(statusRef, (snapshot) => {
  const status = snapshot.val();
  document.getElementById("status").innerText = `Status: ${status}`;
});

window.lightOn = function () {
  set(ref(db, "lightStatus"), "ON")
    .then(() => {
      console.log("Command sent to Firebase");
    })
    .catch((error) => {
      console.error("Error sending command to Firebase", error);
    });
};

window.lightOff = function () {
  set(ref(db, "lightStatus"), "OFF")
    .then(() => {
      console.log("Command sent to Firebase");
    })
    .catch((error) => {
      console.error("Error sending command to Firebase", error);
    });
};

// Function to process leaf health using TensorFlow.js (open-source model)

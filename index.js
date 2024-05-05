const myNameEl = document.querySelector("h1");
myNameEl.innerHTML = "Click to load"

const myName = "A\u200bmr O\u200bjjeh";
let myNameI = 0;

const h2 = document.querySelector("h2");
const h3 = document.querySelector("h3");

const audio = [new Audio("a.wav"), new Audio("inter1.wav"),
new Audio("m.wav"),
new Audio("r.wav"),
new Audio("space.wav"),
new Audio("O.wav"), new Audio("inter2.wav"),
new Audio("j.wav"),
new Audio("j-2.wav"),
new Audio("e.wav"),
new Audio("h.wav")]

const beep = new Audio("beep.wav");

document.body.addEventListener("click", init);
document.body.addEventListener("keypress", init);

h2.classList.add("hidden");
h3.classList.add("hidden");

h3.innerText = new Date().toDateString();

function blink() {
  const carretEl = document.querySelector("#carret");
  if (carretEl) {
    carretEl.classList.toggle("hidden");
    setTimeout(blink, 500);
  }
}

function carret() {
  return `<span id="carret"></span>`
}

function type() {
  const text = document.createTextNode(myName.charAt(myNameI));
  const carretEl = document.querySelector("#carret");
  audio[myNameI].addEventListener("ended", function() {
    if (text.textContent != "\u200b") {
      myNameEl.insertBefore(text, carretEl);
    }
    myNameI += 1;
    if (myNameI != myName.length) {
      setTimeout(type, 200);
    } else {
      beep.addEventListener("ended", function() {
        h2.classList.remove("hidden");
        h3.classList.remove("hidden");
        myNameEl.removeChild(carretEl);
      });
      beep.play();
    }
  });
  audio[myNameI].play();
}

function init() {
  document.body.removeEventListener("click", init);
  document.body.removeEventListener("keypress", init);
  setTimeout(blink, 500);
  myNameEl.innerHTML = carret();
  setTimeout(type, 500);
}


var onManipulator = function( obj ) {
  var value = obj.value;
  console.info(value);
}

var Manipulator = function( options ) {
  var manip_element = document.createElement("div");
  manip_element.className = "manipulator";

  var slider = document.createElement("input");
  slider.setAttribute("id", options.id);
  slider.setAttribute("type", "range");
  slider.setAttribute("min", options.min);
  slider.setAttribute("max", options.max);
  slider.setAttribute("step", options.step);
  slider.setAttribute("onInput", "onManipulator(this)");

  var label = document.createElement("label");
  label.setAttribute("for", options.id);
  label.innerHTML = options.id;

  manip_element.appendChild(label);
  manip_element.appendChild(slider);

  document.body.appendChild(manip_element);
}

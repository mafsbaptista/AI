const server_url = "https://homecontrolserver.herokuapp.com";

function init() {
  const inputs = $('div').find('input');
  let fields;
  for(let i in inputs){
    if(inputs[i].type === 'checkbox') {
      fields = getFields(inputs[i].name);
      if (fields.length == 3) {
        getStateCheckBox(inputs[i], fields[0], fields[1], fields[2]);
      } else {
        //Get Server fields[0], fields[1]
      }
    } else if (inputs[i].type === 'range') {
      let fields = getFields(inputs[i].name);
      getStateRange(inputs[i], fields[0], fields[1], fields[2]);

    }
  }
}


function getStateCheckBox(checkbox, division, type, object) {
  const request_url = server_url + "/" + division + "/" + type + "/" + object;
  const headers = {
    Accept: 'application/json',
    'Content-Type': 'application/json',
  };

  const params = {
    method: "GET",
    mode: "cors",
  };

  fetch(request_url, {
    headers,
    params
  })
    .then(response => response.json())
    .then(response => {
      checkbox.checked = response >= 1;
    });
}

function getStateRange(range, division, type, object) {
  let output = document.getElementById(range.id + "_range");
  console.log(output);
  const request_url = server_url + "/" + division + "/" + type + "/" + object;
  const headers = {
    Accept: 'application/json',
    'Content-Type': 'application/json',
  };

  const params = {
    method: "GET",
    mode: "cors",
  };

  fetch(request_url, {
    headers,
    params
  })
  .then(response => response.json())
  .then(response => {
    console.log(range);
    console.log(response);
    range.value = response;
    output.innerHTML = response;
  });
}

function changeCheckBoxState(checkbox, division, type, object) {
  const request_url = server_url + "/" + division + "/" + type + "/" + object;

  const newstate = (checkbox.checked ? "1" : "0");
  const state = {state: newstate };

  const request = new Request(request_url, {
    method: 'POST',
    mode: 'cors',
    redirect: 'follow',
    headers: new Headers({
      'Content-Type': 'application/json',
    }),
    body: JSON.stringify(state),
  });

  fetch(request)
  .then(response => response.json());
}

function getFields(string) {
  return string.split(',');
}

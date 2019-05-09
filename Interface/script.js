const server_url = "https://homecontrolserver.herokuapp.com";

function init() {
  const inputs = $('div').find('input');
  for(let i in inputs){
    if(inputs[i].type === 'checkbox') {
      let fields = getFields(inputs[i].name);
      if (fields.length == 3) {
        getStateCheckBox(inputs[i], fields[0], fields[1], fields[2]);
      } else {
        //Get Server fields[0], fields[1]
      }
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

function changeState(checkbox, division, type, object) {
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

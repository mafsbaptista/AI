const server_url = "http://homecontrolserver.herokuapp.com";

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

function getFields(string) {
  let fields = string.split(',');
  console.log(fields);
  return fields;
}

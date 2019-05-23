import React, { Component } from 'react';
import {
  Layout, Tabs, Row, Col, Switch,
} from 'antd';
import "antd/dist/antd.css";
import '../css/App.css';

const { Header, Content } = Layout;
const TabPane = Tabs.TabPane;

const server_url = "https://homecontrolserver.herokuapp.com";

class App extends Component {

  state = {
    day_time: false,
    general: {
      people: true,
    },
    masterbedroom: {
      people: true,
    },
    kitchen: {
      people: true,
    }
  };

  render() {
    const {
      day_time, masterbedroom, kitchen, general,
    } = this.state;

    return (
      <Layout>
        <Header>
          <div className="logo">
            <img src={"homecontrol_logo.png"} alt={"Logo"} />
          </div>
          <h2>Control Panel</h2>
        </Header>
        <Content>
          <div className="card-container">
            <Tabs type="card" defaultActiveKey="1">
              <TabPane tab="General" key="1">
                <Row justify="space-around" align="middle">
                  <Col span={12}>
                    <Switch checked={general.people} onClick={() => this.sendSensorInformation('general', 'people')}/>
                    People at Home
                  </Col>
                  <Col span={12}>
                    <Switch checked={day_time} onClick={() => this.sendSensorInformation('general', 'day_time')}/>
                    Day Time
                  </Col>
                </Row>
              </TabPane>
              <TabPane tab="Master Bedroom" key="2">
                <Row justify="space-around" align="middle">
                  <Col span={12}>
                    <Switch checked={masterbedroom.people} onClick={() => this.sendSensorInformation('masterbedroom', 'people')}/>
                    People in the room
                  </Col>
                </Row>
              </TabPane>
              <TabPane tab="Kitchen" key="3">
                <Row justify="space-around" align="middle">
                  <Col span={12}>
                    <Switch checked={kitchen.people} onClick={() => this.sendSensorInformation('kitchen', 'people')}/>
                    People in the room
                  </Col>
                </Row>
              </TabPane>
            </Tabs>
          </div>
        </Content>
      </Layout>
    )
  }

  /**
   * Sends sensor info to server.
   * @return {void}
   * @param division
   * @param event
   */
  sendSensorInformation(division, event) {
    if (event === 'day_time') {
      //Change to day
      if(this.state.day_time === false) {
        App.TurnOffLights('masterbedroom');
        App.TurnOffLights('kitchen');
      } else { //Change to night
        if (this.state.masterbedroom.people === true) {
          App.TurnOnLights('masterbedroom');
        }
        if (this.state.kitchen.people === true) {
          App.TurnOnLights('kitchen');
        }
      }
      this.setState({day_time: !this.state.day_time});
    }

    if (event === 'people') {
      const current_people = this.state[division]['people'];
      //All divisions
      if (division === 'general') {
        if (current_people) {
          App.TurnOffLights("masterbedroom");
          App.TurnOffLights("kichen");
          App.TurnOffHeating("masterbedroom");
          App.TurnOffHeating("kitchen");
          App.TurnOffAppliances('masterbedroom');
          this.setState({"masterbedroom": {...this.state["masterbedroom"], people: false}});
          this.setState({["kitchen"]: {...this.state["kitchen"], people: false}});
        }
        this.setState({"general": {...this.state["general"], people: !this.state["general"].people}});
      }
      //Individual divisions
      else {
        if (current_people) {
          App.TurnOffLights(division);
          App.TurnOffHeating(division);
          if (division !== 'kitchen') {
            App.TurnOffAppliances(division);
          }
        } else {
          App.TurnOnLights(division);
        }
        this.setState({[division]: {...this.state[division], people: !this.state[division].people}});
      }
    }
  }

  static TurnOffLights(division) {
      const request_url = server_url + "/" + division + "/lights";
      const state = {state: "0" };

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
      .then(function () {
      });
    }

  static TurnOnLights(division) {
    const request_url = server_url + "/" + division + "/lights/ceiling_lamp";
    const state = {state: "1" };

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
    .then(function () {
    });
  }

  static TurnOffHeating(division) {
    const request_url = server_url + "/" + division + "/temperature/heating_device";
    const state = {state: "0" };

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
    .then(function () {
    });
  }

  static TurnOffAppliances(division) {
    const request_url = server_url + "/" + division + "/appliances";
    const state = {state: "0" };

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
    .then(function () {
    });
  }
}

export default App;

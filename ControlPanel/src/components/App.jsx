import React, { Component } from 'react';
import {
  Layout, Tabs, Row, Col, Switch,
} from 'antd';
import "antd/dist/antd.css";
import '../css/App.css';

const { Header, Content } = Layout;
const TabPane = Tabs.TabPane;

class App extends Component {
  /**
   * Sends sensor info to server.
   * @param {number} id - licenseid
   * @return {void}
   */
  sendSensorInformation() {

  }
  render() {
    return (
      <Layout>
        <Header>
          <div className="logo">
            <img src={"ist.jpg"} />
          </div>
          <h2>Control Panel</h2>
        </Header>
        <Content>
          <div className="card-container">
            <Tabs type="card" defaultActiveKey="1">
              <TabPane tab="Master Bedroom" key="1">
                <Row justify="space-around" align="middle">
                  <Col span={12}>
                    <Switch defaultChecked />
                    People in the room
                  </Col>
                  <Col span={12}>
                    <Switch />
                    Night Time
                  </Col>
                </Row>
                <Row justify="space-around" align="middle">
                  <Col span={12}>
                    <Switch />
                    Sensor
                  </Col>
                  <Col span={12}>
                    <Switch defaultChecked />
                    Sensor
                  </Col>
                </Row>
              </TabPane>
              <TabPane tab="Kitchen" key="2">
                <Row justify="space-around" align="middle">
                  <Col span={12}>
                    <Switch defaultChecked />
                    People in the room
                  </Col>
                  <Col span={12}>Sensor</Col>
                </Row>
              </TabPane>
            </Tabs>
          </div>
        </Content>
      </Layout>
    )
  }
}

export default App;

const { SerialPort } = require('serialport')
const { Kafka, Partitioners } = require('kafkajs');
const kafkaServer = '173.249.14.91:29092';

const topicCount = 'mes_CounterIOTRequest';
const topicAlive = 'mes_AliveIOTRequest';
const topicBegin = 'mes_StartIOTRequest';
const topicStop = 'mes_StopIOTRequest';

const portName = 'COM5';
let intervalMillisecond = 60000;
let expectedDataCount = 17;
let lastMilliseconds = new Date().getTime();
let receivedData = [];

let device = [
  {
    hexArray: [0x03, 0x03, 0x01, 0xAA],
    preIndexs: 0,
    indexs: 0,
    lastedBtn: '0',
    status: 0,
    jsonDataAlive: {},
    jsonDataCount: {},
    alive: false
  },
  {
    hexArray: [0x04, 0x04, 0x01, 0xAA],
    preIndexs: 0,
    indexs: 0,
    lastedBtn: '0',
    status: 0,
    jsonDataAlive: {},
    jsonDataCount: {},
    alive: false
  },
  {
    hexArray: [0x04, 0x04, 0x01, 0xAA],
    preIndexs: 0,
    indexs: 0,
    lastedBtn: '0',
    status: 0,
    jsonDataAlive: {},
    jsonDataCount: {},
    alive: false
  },
];

// Tạo đối tượng SerialPort với cổng và các tùy chọn
const port = new SerialPort({
  path: portName,
  baudRate: 115200, // Tốc độ baud
  dataBits: 8, // Số bit dữ liệu
  parity: 'none', // Kiểm tra chẵn/lẻ
  stopBits: 1, // Số bit dừng
});

async function produceMessage(topic, jsonData) {
  const kafka = new Kafka({
    clientId: 'TEST',
    brokers: [kafkaServer],
  });

  const producer = kafka.producer({
    createPartitioner: Partitioners.LegacyPartitioner,
  });

  await producer.connect();

  const jsonString = JSON.stringify(jsonData);
  const buffer = Buffer.from(jsonString, 'utf8');
  lastMilliseconds = jsonData.currentMillisecond;

  await producer.send({
    topic: topic,
    messages: [{ value: buffer }],
  });

  await producer.disconnect();
}

// Lắng nghe sự kiện 'open' khi cổng serial đã mở

function sendAndReceiveData(i) {
  if (i >= device.length) {
    i = 0;
  }

  console.log('Thiết bị thứ: ', i);
  const currentDevice = device[i]; // Lấy đối tượng hiện tại từ mảng device
  const arraySend = currentDevice.hexArray;
  port.write(Buffer.from(arraySend));

  const dataListener = (data) => {
    receivedData = receivedData.concat([...data]);

    if (receivedData.length === expectedDataCount) {
      currentDevice.alive = true;
      const combinedValue = (receivedData[10] << 8) | receivedData[11];
      let machineCode = receivedData[0].toString(16) + receivedData[1].toString(16);
      let currentMillisecond = new Date().getTime();

      if (combinedValue < currentDevice.preIndexs) {
        currentDevice.indexs += combinedValue + 65535 - currentDevice.preIndexs + 1;
      } else {
        currentDevice.indexs += combinedValue - currentDevice.preIndexs;
      }

      currentDevice.preIndexs = combinedValue;

      if (receivedData[13].toString(16) != currentDevice.lastedBtn) {
        currentDevice.lastedBtn = receivedData[13].toString(16);
        if (receivedData[13].toString(16) == '1') {
          currentDevice.indexs = 0;
          currentDevice.preIndexs = combinedValue;
          jsonDataBegin = {
            "machineCode": machineCode,
            "currentMillisecond": currentMillisecond,
            "intervalMillisecond": intervalMillisecond,
          }

          produceMessage(topicBegin, jsonDataBegin)
            .then(() => {
              console.log('Message begin successfully.');
            })
            .catch((error) => {
              console.error('Error producing message:', error);
            });
        } else if (receivedData[13].toString(16) == '0') {
          jsonDataStop = {
            "machineCode": machineCode,
            "currentMillisecond": currentMillisecond,
            "lastMillisecond": lastMilliseconds,
            "lastIndex": currentDevice.indexs,
          }
          produceMessage(topicStop, jsonDataStop)
            .then(() => {
              console.log('Message stop successfully.');
            })
            .catch((error) => {
              console.error('Error producing message:', error);
            });
        }
      }

      if (receivedData[4].toString(16) == '1') {
        currentDevice.status = 0;
      } else if (receivedData[6].toString(16) == '1') {
        currentDevice.status = 1;
      } else if (receivedData[8].toString(16) == '1') {
        currentDevice.status = 2;
      }

      console.log('Số lượng sản phẩm ghi nhận được: ', currentDevice.indexs);
      currentDevice.jsonDataCount = {
        "machineCode": machineCode,
        "currentMillisecond": currentMillisecond,
        "lastMillisecond": lastMilliseconds,
        "index": currentDevice.indexs,
      }

      currentDevice.jsonDataAlive = {
        "machineCode": machineCode,
        "currentMillisecond": currentMillisecond,
        "status": currentDevice.status,
      }

      receivedData = [];
    }
  };

  setTimeout(() => {
    port.off('data', dataListener);
    sendAndReceiveData(i + 1); // Gọi đệ quy để chạy chương trình cho đối tượng tiếp theo
  }, 5000);
  port.on('data', dataListener);
}

sendAndReceiveData(0);

// Count && Alive
setInterval(() => {
  for (let k = 0; k < device.length; k++) {
    if (device[k].alive == true) {
      device[k].alive = false;
      produceMessage(topicCount, device[k].jsonDataCount)
        .then(() => {
          console.log('Message count successfully.');
        })
        .catch((error) => {
          console.error('Error producing message:', error);
        });
      produceMessage(topicAlive, device[k].jsonDataAlive)
        .then(() => {
          console.log('Message alive successfully.');
        })
        .catch((error) => {
          console.error('Error producing message:', error);
        });
    }
  }
}, 60000);

// Đóng cổng Serial Port khi kết thúc chương trình
process.on('SIGINT', () => {
  port.close((error) => {
    if (error) {
      console.error('Lỗi khi đóng cổng Serial Port:', error);
    } else {
      console.log('Cổng Serial Port đã đóng.');
      process.exit();
    }
  });
});
const { SerialPort } = require('serialport')
const { Kafka, Partitioners } = require('kafkajs');
const kafkaServer = '173.249.14.91:29092';

const topicCount = 'mes_CounterIOTRequest';
const topicAlive = 'mes_AliveIOTRequest';
const topicBegin = 'mes_StartIOTRequest';
const topicStop = 'mes_StopIOTRequest';

const portName = 'COM3';
let expectedDataCount = 17;

let preIndexs = 0;
const hexArray = ['0x05', '0x05', '0x01', '0xAA']; // sẽ thay đổi
let indexs = 0;
let lastedBtn = '0';
let status;
let receivedData = []; 
let intervalMillisecond = 60000;
let lastMilliseconds = new Date().getTime();

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
port.on('open', () => {

  setInterval(() => {
    port.write(Buffer.from(hexArray));
  }, 5000);
  
  console.log('Cổng serial đã mở.');
  port.on('data', (data) => {
    receivedData = receivedData.concat([...data]); // Gộp dữ liệu vào mảng receivedData
    
    if (receivedData.length === expectedDataCount) {
      const combinedValue = (receivedData[10] << 8) | receivedData[11]; 
      let machineCode = receivedData[0].toString(16) + receivedData[1].toString(16);
      let currentMillisecond = new Date().getTime();

      if (combinedValue < preIndexs){
        indexs += combinedValue + 65535 - preIndexs + 1;
      } else {
        indexs += combinedValue - preIndexs;
      }
      
      preIndexs = combinedValue;
      
      if (receivedData[13].toString(16) != lastedBtn){
        lastedBtn = receivedData[13].toString(16);
        if (receivedData[13].toString(16) == '1'){
          indexs = 0;
          preIndexs = combinedValue;
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
        } else if (receivedData[13].toString(16) == '0'){
          jsonDataStop = {
            "machineCode": machineCode,     
            "currentMillisecond": currentMillisecond,     
            "lastMillisecond": lastMilliseconds,     
            "lastIndex": indexs, 
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
      
      if (receivedData[4].toString(16) == '1'){
        status = 0;
      } else if (receivedData[6].toString(16) == '1'){
        status = 1;
      } else if (receivedData[8].toString(16) == '1'){
        status = 2;
      }

      console.log('Mảng byte nhận được (hex):', combinedValue);
      jsonDataCount = {
        "machineCode": machineCode,
        "currentMillisecond": currentMillisecond,
        "lastMillisecond": lastMilliseconds,
        "index": indexs, 
      }
      
      jsonDataAlive = {
        "machineCode": machineCode,
        "currentMillisecond": currentMillisecond,
        "status": status,   
      }
      
      receivedData = []; 
    }
  });
});

// Count
setInterval(() => {
  produceMessage(topicCount, jsonDataCount)
    .then(() => {
      console.log('Message count successfully.');
    })
    .catch((error) => {
      console.error('Error producing message:', error);
    });
}, 60000);

// Alive
setInterval(() => {
  produceMessage(topicAlive, jsonDataAlive)
    .then(() => {
      console.log('Message alive successfully.');
    })
    .catch((error) => {
      console.error('Error producing message:', error);
    });
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
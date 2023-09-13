const { Kafka } = require('kafkajs');

// Khởi tạo một Kafka producer
const kafka = new Kafka({
  clientId: 'my-producer', // Đặt tên cho producer
  brokers: ['194.233.92.193:9092'], // Kafka broker address
});

const producer = kafka.producer();

// Khởi tạo hàm async để gửi tin nhắn
async function sendKafkaMessage() {
  const topic = 'usm_IOTRequest'; // Đặt tên topic
  const message = {
    type: 1,
    machineCode: 'BI0001',
    currentMillisecond: 1686082323858,
    intervalMillisecond: 60000,
  };

  try {
    await producer.connect(); // Kết nối đến Kafka broker

    // Gửi tin nhắn tới Kafka topic
    await producer.send({
      topic,
      messages: [
        {
          value: JSON.stringify(message), // Chuyển đổi đối tượng thành chuỗi JSON
        },
      ],
    });

    console.log('Tin nhắn đã được gửi thành công.');
  } catch (error) {
    console.error('Lỗi khi gửi tin nhắn:', error);
  } finally {
    await producer.disconnect(); // Đóng kết nối Kafka producer
  }
}

// Gọi hàm để gửi tin nhắn
sendKafkaMessage();

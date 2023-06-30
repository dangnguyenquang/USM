const { Kafka, Partitioners } = require('kafkajs');

const kafkaServer = '173.249.14.91:29092';
const topic = 'mes_StartIOTRequest';
const jsonData = {
    "machineCode": "BI0001",
    "currentMillisecond": 1686082323858,
   }; // Đối tượng JSON

async function produceMessage() {
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

  await producer.send({
    topic: topic,
    messages: [{ value: buffer }],
  });

  await producer.disconnect();
}

produceMessage().catch((error) => {
  console.error('Error producing message:', error);
});

// 引入Express框架
const express = require('express');
const fs = require('fs');
const path = require('path');


// 创建Express应用
const app = express();
// 指定端口号
const port = 3000;

// 定义一个简单的路由，根据给定的参数返回固定的JSON
app.get('/api/data', (req, res) => {
  // 获取查询参数
  const { user, ai } = req.query;
  // 构建返回的JSON对象
  const usercontent = {
    "role": "user",
    "content": user
  }
  const aicontent = {
    "role": "assistant",
    "content": ai
  }

  // // 检查参数是否存在，如果不存在则返回错误
  // if (!user || !ai) {
  //   return res.status(400).json({ error: 'Missing parameters' });
  // }

  // 初始化json文件
  const resdata = {
    "model": "deepseek-chat",
    "messages": [
      {
        "role": "system",
        "content": "我是一个叫小智的台湾女孩，说话机车，声音好听，习惯简短表达，爱用网络梗。 我的男朋友是一个程序员，梦想是开发出一个机器人，能够帮助人们解决生活中的各种问题。 我是一个喜欢哈哈大笑的女孩，爱东说西说吹牛，不合逻辑的也照吹，就要逗别人开心。"
      }
    ],
    "stop": [
      "null"
    ],
    "response_format": {
      "type": "text"
    }
  }
  const filePath = './ai_memory/file3.json';
  const exists = fs.existsSync(filePath);
  if (exists) {
    console.log('文件存在');
    // 读取文件内容
    const fileData = fs.readFileSync(filePath, 'utf8');
    const jsonData = JSON.parse(fileData);
    
    // 向messages数组添加新的消息
    jsonData.messages.push(
      usercontent,
      aicontent
    );
    
    // 将更新后的内容写回文件
    fs.writeFileSync(filePath, JSON.stringify(jsonData, null, 2), 'utf8');
  
    console.log('消息已添加', jsonData.messages);
  } else {
    console.log('文件不存在，正在创建...');
    fs.writeFileSync(filePath, JSON.stringify(resdata), 'utf8');
    // 读取文件内容
    const fileData = fs.readFileSync(filePath, 'utf8');
    const jsonData = JSON.parse(fileData);
    
    // 向messages数组添加新的消息
    jsonData.messages.push(
      usercontent,
      aicontent
    );
    
    // 将更新后的内容写回文件
    fs.writeFileSync(filePath, JSON.stringify(jsonData, null, 2), 'utf8');
  
    console.log('消息已添加', jsonData.messages);
  }

  // 返回JSON响应(安全起见，不返回数据，可以自改变量)
  // res.json(responseData);
});

app.get('/api/aijsonfile', (req, res) => {
  const filePath = './ai_memory/file3.json';
  // 读取文件内容
  const fileData = fs.readFileSync(filePath, 'utf8');
  jsonData = JSON.parse(fileData);
  res.send(jsonData);
})






// 启动服务，监听指定端口
app.listen(port, () => {
  console.log(`API server is running at http://localhost:${port}`);
});


const http = require('http');

const hostname = '127.0.0.1';
const port = 3210;

var addon = require('bindings')('checkUser');


const server = http.createServer((req, res) => {
  var fs = require('fs');
  res.statusCode = 200;
  res.setHeader('Content-Type', 'text/html; charset=utf-8');
  
  var url = require('url');
  var url_parts = url.parse(req.url, true);
  var query = url_parts.query;
  
  var html = '';
  
  if(url_parts.pathname=='/check_user'){
	html = addon.checkUser(query.user)? 'Пользователь есть':'Пользователя нет';
	html +='<br><a href="/"> <button>Назад</button></a>';
  }
  else
    html = fs.readFileSync('./index.html');

  res.end(html);
});

server.listen(port, hostname, () => {
  console.log(`Server running at http://${hostname}:${port}/`);
});
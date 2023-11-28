const String headPage = "<html lang=\"en\">\
<head>\
    <title>Green Salad - Gestor de horta hidroponica</title>\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
    <style>.sucess{border: 1px solid green;background-color: greenyellow;padding: 5px;margin-top: 5px;margin-bottom: 5px;}.title,a.active{font-weight:bolder}.by,.sub_div,select,table{width:100%}.by,.title,button{color:#fff}body{margin:0;padding:0;font-family:Verdana,Geneva,Tahoma,sans-serif}table{border-collapse:collapse}td{padding:2px}th{width:5%}.sub_div{position:absolute;bottom:0}.title{background-color:#000;height:50px;font-size:larger}.by,.sub_title,input{height:25px}.sub_title{background-color:#f2f2f2}.by{background-color:#000}a{color:#d2691e}h2{font-size:medium;border-bottom:1px solid #000;margin-bottom:5px}select{padding:5px;height:40px}input{height: 40px;width:96%;padding:5px}button{width:99%;padding:6px;margin:2px;height:40px;background-color:#d2691e}</style>\
</head>\
<body>\
    <table>\
        <tr class=\"title\">\
            <th></th>\
            <td>Green Salad</td>\
            <th></th>\
        </tr>\
        <tr class=\"sub_title\">\
            <th></th>\
            <td>\
                <a href=\"/\">Home</a>\
                | <a href=\"/handleFormConfig\">Configuracao</a>\
            </td>\
            <th></th>\
        </tr>\
        <tr >\
            <th></th>\
            <td>";
const String footPage = " </td>\
            <th></th>\
        </tr>\
    </table>\
    <div class=\"sub_div\">\
        <table >\
            <tr class=\"by\">\
                <th></th>\
                <td>Desenvolvido por  <a href=\"\">Rafael Aguiar</a></td>\
                <th></th>\
            </tr>\
        </table>\
    </div>\
 </body>\
</html>";



void handleIndex(){
  String page = headPage;
  if (msgStatus.length() > 0) { page += "<div class=\"sucess\">"+msgStatus+"</div>";  msgStatus ="";}
  page += "\
  <h1>Gestor de horta hidroponica.</h1>\
  <p>Gerencie a irrigacao e manutecao do reservatorio de agua com esse dispositivo.</p>";
  page += footPage;

  server.send(200, "text/html", page); 
}

void handleFormConfig(){
  String page = headPage+"<h1>Configuracoes</h1>\
  <form method=\"post\" action=\"/save\" onsubmit=\"return concatenar()\">\
  <h2>Horarios de ativacao</h2>\
  <table>\
    <tr>\
        <td>0 </td>\
        <td>1 </td>\
        <td>2 </td>\
        <td>3 </td>\
        <td>4 </td>\
        <td>5 </td>\
    </tr>\
    <tr>\
        <td><input id=\"h0\"></td>\
        <td><input id=\"h1\"></td>\
        <td><input id=\"h2\"></td>\
        <td><input id=\"h3\"></td>\
        <td><input id=\"h4\"></td>\
        <td><input id=\"h5\"></td>\
    </tr>\
    <tr>\
        <td>6 </td>\
        <td>7 </td>\
        <td>8 </td>\
        <td>9 </td>\
        <td>10 </td>\
        <td>11 </td>\
    </tr>\
    <tr>\
        <td><input id=\"h6\"></td>\
        <td><input id=\"h7\"></td>\
        <td><input id=\"h8\"></td>\
        <td><input id=\"h9\"></td>\
        <td><input id=\"h10\"></td>\
        <td><input id=\"h11\"></td>\
    </tr>\
    <tr>\
        <td>12 </td>\
        <td>13 </td>\
        <td>14 </td>\
        <td>15 </td>\
        <td>16 </td>\
        <td>17 </td>\
    </tr>\
    <tr>\
        <td><input id=\"h12\"></td>\
        <td><input id=\"h13\"></td>\
        <td><input id=\"h14\"></td>\
        <td><input id=\"h15\"></td>\
        <td><input id=\"h16\"></td>\
        <td><input id=\"h17\"></td>\
    </tr>\
    <tr>\
        <td>18 </td>\
        <td>19 </td>\
        <td>20 </td>\
        <td>21 </td>\
        <td>22 </td>\
        <td>23 </td>\
    </tr>\
    <tr>\
        <td><input id=\"h18\"></td>\
        <td><input id=\"h19\"></td>\
        <td><input id=\"h20\"></td>\
        <td><input id=\"h21\"></td>\
        <td><input id=\"h22\"></td>\
        <td><input id=\"h23\"></td>\
    </tr>\
  </table>\
  <br>\
  <button>Salvar</button>\
  <textarea style=\"display: none;\" id=\"horarios_funcionamento\" name=\"horarios_funcionamento\">"+MyConfig.horarios_funcionamento+"</textarea>\
  </form>\
  <script>\
  function str_split(string, split_length) {\
    if (split_length === null) {\
      split_length = 1;\
    }\
    if (string === null || split_length < 1) {\
      return false;\
    }\
    string += '';\
    var chunks = [],\
      pos = 0,\
      len = string.length;\
    while (pos < len) {\
      chunks.push(string.slice(pos, pos += split_length));\
    }\
    return chunks;\
  }\
  function concatenar()\
  {\
      document.getElementById('horarios_funcionamento').value = '';\
      for(var i =0 ; i<=23 ; i++)\
      {\
            if (document.getElementById('h'+i).value.length < 4 || document.getElementById('h'+i).value.length > 4){    \        
              alert('Falha de preenchimento no campo de '+i+' hora(s)!');   \    
              return false;\        
          }\              
          document.getElementById('horarios_funcionamento').value += document.getElementById('h'+i).value;\
          document.getElementById('h'+i).disabled  = true;\
      }\
  }\
  var arrayVals =  str_split(document.getElementById('horarios_funcionamento').value, 4 );\
  arrayVals.forEach(function(value, i) {\
      document.getElementById('h'+i).value = value;\
  })\
  </script>\
  "+footPage; 
  server.send(200, "text/html", page); 
}
void handleSaveConfig(){
  
  String horarios_funcionamento = server.arg("horarios_funcionamento");


  horarios_funcionamento.toCharArray(MyConfig.horarios_funcionamento, horarios_funcionamento.length() +1);


  
  EEPROM.put(0, MyConfig);
  EEPROM.commit();
  
  msgStatus = "Configuracao salva com sucesso.";
  handleIndex(); 

}
void handleNotFound(){
  server.send(404, "text/html", headPage+"Pagina nao encontrada"+footPage); 
} 

function __templateRUN()
{
  local USER=$1
  # Coloque aqui como executar o seu programa
  # se o seu programa recebe o USERNAME por parâmetro pode passar já
  ./prog $USER
}

function __templateGENNAME()
{
  local USER=$1
  #Se o seu programa pede o username depois da execução comente a linha de
  #return
  #return

  # Coloque aqui todas as linhas necessárias para definir o username
  # no exemplo abaixo o programa apenas pede para que se digite o nome do
  # usuário e tecle enter
  echo $USER
}

function __templateEXIT()
{
  # comando para sair do seu programa
  echo exit
}

function __templateSENDMSG()
{
  local TO="$1"
  local MSG="$2"

  # template para o envio de mensagem, se o seu programa pede primeiro o
  # username em uma linha e depois a mensagem em outra siga como abaixo
  echo "$TO"
  echo "$MSG"

  #Se o seu programa pede qu ena mesma linha tenha o destinatário e a
  #mensagem, comente as linhas acima e descomente abaixo
  # echo "$TO:$MSG"
}

function __templateBROADCAST()
{
  local MSG="$1"

  #como enviar mensagem do tipo broadcast?
  #abaixo ele faz o uso do template de mensagem, utilizando o destinatário
  #como "all"
  __templateSENDMSG "all" "$MSG"
}

function __templateCRIARSALA()
{
  #template para criar sala
  local SALA=$1
  echo "$SALA"
}

function __templateSENDMSGSALA()
{
  local SALA="$1"
  local MSG="$2"
  #template enviar mensagem em sala
  echo $SALA
  echo $MSG
}

function __templateJOINSALA()
{
  local SALA=$1
  echo $SALA
  echo join
}

function __templateEXITSALA()
{
  local SALA=$1
  echo $SALA
  echo exit
}

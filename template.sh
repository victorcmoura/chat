function __templateRUN()
{
  ./run
}

function __templateGENNAME()
{
  local USER=$1
  #Se o seu programa pede o username depois da execução comente a linha de
  #return
  return

  # Coloque aqui todas as linhas necessárias para definir o username
  # no exemplo abaixo o programa apenas pede para que se digite o nome do
  # usuário e tecle enter
  # echo $USER
}

function __templateEXIT()
{
  # comando para sair do seu programa
  # echo exit
  echo ^[
  echo 5
  echo 2
}

function __templateSENDMSG()
{
  local TO="$1"
  local MSG="$2"

  # template para o envio de mensagem, se o seu programa pede primeiro o
  # username em uma linha e depois a mensagem em outra siga como abaixo
  # echo "$TO"
  # echo "$MSG"

  #Se o seu programa pede qu ena mesma linha tenha o destinatário e a
  #mensagem, comente as linhas acima e descomente abaixo
  # echo "$TO:$MSG"
  echo "1"

  if [ $TO = "all" ]
  then
    echo "2"
  else
    echo "0"
  fi
  
  echo "i ($MSG)"
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
  echo 1
  echo 3
  echo "$SALA"
}

function __templateSENDMSGSALA()
{
  local SALA="$1"
  local MSG="$2"
  #template enviar mensagem em sala
  echo 1
  echo 1

  echo "i ($MSG)"
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

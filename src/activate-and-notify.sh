#!/bin/bash

WAIT=5

# Este script tem de correr com o user bonitomail para aceder 'a base de dados.

export LANG=en_US.UTF-8

#echo "Starting in $WAIT seconds ..."
sleep $WAIT

# Para os pedidos já confirmados.
# Aprovisiona os endereços na tabela utilizada pelo Postfix.
psql -A -t -q -F" " \
	-c "select * from execute_confirmed_requests()" \
	bonitomail | while read VIRTUAL REAL
do
	echo $VIRTUAL $REAL

# Envia a mensagem de confirmação.

ORIGIN=sistema@mail.bonito.pt
SUBJECT="[bonito.pt] Encaminhamento ativo."
echo "Olá!


O endereço virtual $VIRTUAL foi ativado.
Já estamos a encaminhar as mensagens para $REAL.

Lembra-te, podes sempre eliminar o endereço virtual ou atualizar o endereço real.


Cumprimentos.
http://bonito.pt/
" | mail -s "$SUBJECT" -r $ORIGIN $REAL

done

#echo "Stopped."

# EOF

#!/bin/bash

WAIT=5

# Este script tem de correr com o user bonitomail para aceder 'a base de dados.

export LANG=en_US.UTF-8

#echo "Starting in $WAIT seconds ..."
sleep $WAIT

# Gera tokens para registos de pedidos que ainda não têm tokens.
# Obtem os endereços virtual e real e também o token.
psql -A -t -q -F" " \
	-c "update request
		set token = encode( digest(
			aliasemail || realemail
			|| created::text || now(),
			'sha1' ), 'hex' )
		where token is null
		returning aliasemail, realemail, token" \
	bonitomail | while read VIRTUAL REAL TOKEN
do
	echo $VIRTUAL $REAL $TOKEN

# Envia a mensagem com o pedido de confirmação.

ORIGIN=sistema@mail.bonito.pt
SUBJECT="[bonito.pt] Confirma o teu endereço."
echo "Olá!


Queres mesmo criar o endereço virtual $VIRTUAL?
E encaminhar todas as mensagens para este teu endereço $REAL?

Carrega no endereço que se segue para confirmar:
http://bonito.pt/action?action=confirm&token=$TOKEN

Se mudares de ideias, podes sempre eliminar o endereço virtual.


Cumprimentos.
http://bonito.pt/
" | mail -s "$SUBJECT" -r $ORIGIN $REAL

done

#echo "Stopped."
# EOF

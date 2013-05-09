CREATE LANGUAGE plpgsql;

create or replace function generate_encrypted_mailfrom( text, text )
returns setof text as $$
declare
	sk text;
begin
	sk := ( select symetric_key 
		from domain_return_path_keys
		where domain = $2 and 0 = position( $2 in $1 )
		limit 1 );

	if sk is not null then
		return next encode( pgp_sym_encrypt( $1, sk ), 'hex' ) || '@' || $2;
	end if;

	return;
end;
$$
language plpgsql ;

create or replace function obtain_decrypted_rcptto( text, text )
returns setof text as $$
declare
	sk text;
begin
	sk := ( select symetric_key 
		from domain_return_path_keys
		where domain = $2 and 0 < position( $2 in $1 )
		limit 1 );

	if sk is not null then
		return next pgp_sym_decrypt( decode( trim( ( '@' || $2 ) from $1 ), 'hex' ), sk );
	end if;

	return;
exception when OTHERS then return;
end;
$$
language plpgsql ;


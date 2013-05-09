CREATE LANGUAGE plpgsql;

create or replace function new_create_request( text, text )
returns smallint as $$
declare
	rc int;
begin
	if ( select count(1) from email where aliasemail = $1 ) > 0 then
		return -1; -- NEW_CREATE_CODE_VIRTUAL_EXISTS
	elsif ( select count(1) from email where realemail = $2 ) > 0 then
		return -2; -- NEW_CREATE_CODE_REAL_EXISTS
	end if;

	insert into request
		( aliasemail, realemail, action, token, created, confirmed )
		values
		( $1, $2, 'create', NULL, now(), NULL );
	GET DIAGNOSTICS rc = ROW_COUNT;
	if 1 = rc then
		return 0; -- NEW_CREATE_CODE_REAL_EXISTS
	end if;

	return -3; -- NEW_CREATE_CODE_BUG
end;
$$
language plpgsql ;

create or replace function confirm_create_request( text )
returns smallint as $$
declare
	rc int;
begin
	if ( select count(1) from request where token = $1 and confirmed is not null ) > 0 then
		return -1; -- CONFIRM_CREATE_CODE_TOKEN_VALIDATED
	elsif ( select count(1) from request where token = $1 ) != 1 then
		return -2; -- CONFIRM_CREATE_CODE_TOKEN_INVALID
	end if;

	update request set confirmed = now() where action = 'create' and token = $1;
	GET DIAGNOSTICS rc = ROW_COUNT;
	if 1 = rc then
		return 0; -- CONFIRM_CREATE_CODE_OK
	end if;

	return -3; -- CONFIRM_CREATE_CODE_BUG
end;
$$
language plpgsql ;

create or replace function execute_confirmed_requests()
returns setof email as $$
declare
	r email%rowtype;
begin
	-- For all requests confirmed and waiting execution ...
	for r in update request
		-- ... set the 'executed' timestamp ...
		set executed = now()
		where confirmed is not null
			and executed is null
		returning aliasemail, realemail
	loop
		-- ... and for each pair of alias/real email do the following:
		begin
			-- Activate the email forwarding rule.
			insert into email ( aliasemail, realemail )
				values ( r.aliasemail, r.realemail );
		exception when unique_violation then
			-- Go to loop's top without returning this duplicated entry.
			continue;
		end;
		-- Add the inserted entry to the result list.
		return next r;
	end loop;
	return;
exception when OTHERS then return;
end;
$$
language plpgsql ;


CREATE TABLE email (
    aliasemail character varying(256) NOT NULL,
    realemail character varying(256)
);
ALTER TABLE ONLY email
    ADD CONSTRAINT email_pkey PRIMARY KEY (aliasemail);

CREATE TABLE domain_return_path_keys (
    domain character varying(256) NOT NULL,
    symetric_key character varying(256)
);
ALTER TABLE ONLY domain_return_path_keys
    ADD CONSTRAINT domain_return_path_keys_pkey PRIMARY KEY (domain);

CREATE TYPE request_action AS ENUM (
    'create',
    'update',
    'delete'
);

CREATE TABLE request (
    aliasemail character varying(256),
    realemail character varying(256),
    action request_action,
    token character varying(256),
    created timestamp without time zone,
    confirmed timestamp without time zone,
    executed timestamp without time zone
);

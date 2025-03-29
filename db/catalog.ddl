create table if not exists Catalog (
  id integer primary key autoincrement,
  name text not null,
  display_name text not null,
  description text default "",
  repo_url text not null,
  code_location text not null,
  libtype text not null,
  dependencies text,
  special_install_instruction_id int
);

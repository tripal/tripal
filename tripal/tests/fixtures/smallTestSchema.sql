CREATE TABLE regions (
  region_id SERIAL PRIMARY KEY,
  region_name CHARACTER VARYING (25)
);

CREATE TABLE countries (
  country_id CHARACTER (2) PRIMARY KEY,
  country_name CHARACTER VARYING (40),
  region_id INTEGER NOT NULL,
  FOREIGN KEY (region_id) REFERENCES regions (region_id) ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE locations (
  location_id SERIAL PRIMARY KEY,
  street_address CHARACTER VARYING (40),
  city CHARACTER VARYING (30) NOT NULL,
  state_province CHARACTER VARYING (25),
  country_id CHARACTER (2) NOT NULL,
  FOREIGN KEY (country_id) REFERENCES countries (country_id) ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE castles (
  castle_id SERIAL PRIMARY KEY,
  castle_name CHARACTER VARYING (30) NOT NULL,
  location_id INTEGER,
  FOREIGN KEY (location_id) REFERENCES locations (location_id) ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE knights (
  knight_id SERIAL PRIMARY KEY,
  first_name CHARACTER VARYING (20),
  last_name CHARACTER VARYING (25) NOT NULL,
  knighthood_ceremony_date DATE NOT NULL,
  castle_id INTEGER,
  FOREIGN KEY (castle_id) REFERENCES castles (castle_id) ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE ancestors (
  ancestor_id SERIAL PRIMARY KEY,
  first_name CHARACTER VARYING (50) NOT NULL,
  last_name CHARACTER VARYING (50) NOT NULL,
  relationship CHARACTER VARYING (25) NOT NULL,
  knight_id INTEGER NOT NULL,
  FOREIGN KEY (knight_id) REFERENCES knights (knight_id) ON DELETE CASCADE ON UPDATE CASCADE
);

/*Data for the table regions */

INSERT INTO regions(region_id,region_name) VALUES (1,'Vligall');
INSERT INTO regions(region_id,region_name) VALUES (2,'Prawoth');
INSERT INTO regions(region_id,region_name) VALUES (3,'Vaecia');
INSERT INTO regions(region_id,region_name) VALUES (4,'Iucen');
INSERT INTO regions(region_id,region_name) VALUES (5,'Aucai');
INSERT INTO regions(region_id,region_name) VALUES (6,'Aiyiken');
INSERT INTO regions(region_id,region_name) VALUES (7,'Phukaivela');
INSERT INTO regions(region_id,region_name) VALUES (8,'Treiwidin');
INSERT INTO regions(region_id,region_name) VALUES (9,'Reuwitrish');

/*Data for the table countries */
INSERT INTO countries(country_id,country_name,region_id) VALUES ('AF','afreania',2);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('SW','soswayqua',3);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('ZL','zothil',1);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('LJ','lastijan',2);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('FR','froassau',2);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('SQ','shuqua',1);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('AP','aplurg',3);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('AC','ascium',1);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('PC','proa crax',1);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('WP','wespiunia',4);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('IA','iawhain',1);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('CS','casta',3);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('PY','proyae',4);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('UD','udral',3);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('EF','efrad',1);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('WG','whuw grium',3);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('HB','haspeaburg',4);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('KF','kefros',2);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('PV','pruovania',4);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('AT','astesh',1);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('FS','frouy spines',3);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('GG','griul glary',1);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('YK','yuskouvania',2);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('RP','ruplenia',4);
INSERT INTO countries(country_id,country_name,region_id) VALUES ('GR','griucia',4);

/*Data for the table locations */
INSERT INTO locations(location_id,street_address,city,state_province,country_id) VALUES (1400,'2014 Windmill Avenue','Narrow Rill','Deggulir','AF');
INSERT INTO locations(location_id,street_address,city,state_province,country_id) VALUES (1500,'2011 Long Boulevard','Infernal Channel','Korboldihr','AF');
INSERT INTO locations(location_id,street_address,city,state_province,country_id) VALUES (1700,'2004 Grove Row','Slumbrous Run','Khighturuhm','AF');
INSERT INTO locations(location_id,street_address,city,state_province,country_id) VALUES (1800,'147 Vale Route','Turtle River','Ganboldohr','ZL');
INSERT INTO locations(location_id,street_address,city,state_province,country_id) VALUES (2400,'8204 Heart Lane','Northern River',NULL,'ZL');
INSERT INTO locations(location_id,street_address,city,state_province,country_id) VALUES (2500,'Stone Lane','Ganwin Beck','Bhugh Darahl','ZL');
INSERT INTO locations(location_id,street_address,city,state_province,country_id) VALUES (2700,'Storm Way. 7031','Terrenmiota River','Khom Thorim','FS');

/*Data for the table castles */
INSERT INTO castles(castle_id,castle_name,location_id) VALUES (1,'Clafton Fortress',1700);
INSERT INTO castles(castle_id,castle_name,location_id) VALUES (2,'Darpley Castle',1800);
INSERT INTO castles(castle_id,castle_name,location_id) VALUES (3,'Pardwell Fort',1700);
INSERT INTO castles(castle_id,castle_name,location_id) VALUES (4,'Droskyn Fort',2400);
INSERT INTO castles(castle_id,castle_name,location_id) VALUES (5,'Shaldorn Stronghold',1500);
INSERT INTO castles(castle_id,castle_name,location_id) VALUES (6,'Nascombe Fortress',1400);
INSERT INTO castles(castle_id,castle_name,location_id) VALUES (7,'Goodmond Castle',2700);
INSERT INTO castles(castle_id,castle_name,location_id) VALUES (8,'Eaghton Keep',2500);
INSERT INTO castles(castle_id,castle_name,location_id) VALUES (9,'Carnstock Fort',1700);
INSERT INTO castles(castle_id,castle_name,location_id) VALUES (10,'Calford Keep',1700);
INSERT INTO castles(castle_id,castle_name,location_id) VALUES (11,'Eagleview Fortress',1700);

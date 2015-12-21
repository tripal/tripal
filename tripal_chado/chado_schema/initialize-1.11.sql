/* For load_gff3.pl */
insert into organism (abbreviation, genus, species, common_name)
       values ('H.sapiens', 'Homo','sapiens','human');
insert into organism (abbreviation, genus, species, common_name)
       values ('D.melanogaster', 'Drosophila','melanogaster','fruitfly');
insert into organism (abbreviation, genus, species, common_name)
       values ('M.musculus', 'Mus','musculus','mouse');
insert into organism (abbreviation, genus, species, common_name)
       values ('A.gambiae', 'Anopheles','gambiae','mosquito');
insert into organism (abbreviation, genus, species, common_name)
       values ('R.norvegicus', 'Rattus','norvegicus','rat');
insert into organism (abbreviation, genus, species, common_name)
       values ('A.thaliana', 'Arabidopsis','thaliana','mouse-ear cress');
insert into organism (abbreviation, genus, species, common_name)
       values ('C.elegans', 'Caenorhabditis','elegans','worm');
insert into organism (abbreviation, genus, species, common_name)
       values ('D.rerio', 'Danio','rerio','zebrafish');
insert into organism (abbreviation, genus, species, common_name)
       values ('O.sativa', 'Oryza','sativa','rice');
insert into organism (abbreviation, genus, species, common_name)
       values ('S.cerevisiae', 'Saccharomyces','cerevisiae','yeast');
insert into organism (abbreviation, genus, species, common_name)
       values ('X.laevis', 'Xenopus','laevis','frog');
insert into organism (abbreviation, genus, species,common_name) 
       values ('D.discoideum','Dictyostelium','discoideum','dicty');
insert into contact (name) values ('Affymetrix');
insert into contact (name,description) values ('null','null');
insert into cv (name) values ('null');
insert into cv (name,definition) values ('local','Locally created terms');
insert into cv (name,definition) values ('Statistical Terms','Locally created terms for statistics');
insert into db (name, description) values ('null','a fake database for local items');

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'local:null');
insert into cvterm (name,cv_id,dbxref_id) values ('null',(select cv_id from cv where name = 'null'),(select dbxref_id from dbxref where accession='local:null'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'local:computer file');
insert into cvterm (name,cv_id,dbxref_id) values ('computer file', (select cv_id from cv where name = 'null'),(select dbxref_id from dbxref where accession='local:computer file'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'local:glass');
insert into cvterm (name,definition,cv_id,dbxref_id) values ('glass','glass array',(select cv_id from cv where name = 'local'),(select dbxref_id from dbxref where accession='local:glass'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'local:photochemical_oligo');
insert into cvterm (name,definition,cv_id,dbxref_id) values ('photochemical_oligo','in-situ photochemically synthesized oligoes',(select cv_id from cv where name = 'local'),(select dbxref_id from dbxref where accession='local:photochemical_oligo'));

insert into pub (miniref,uniquename,type_id) values ('null','null',(select cvterm_id from cvterm where name = 'null'));
insert into db (name, description) values ('GFF_source', 'A collection of sources (ie, column 2) from GFF files');

insert into db (name) values ('ATCC');

insert into db (name) values ('DB:refseq');
insert into db (name) values ('DB:genbank');
insert into db (name) values ('DB:EMBL');
insert into db (name) values ('DB:TIGR');
insert into db (name) values ('DB:ucsc');
insert into db (name) values ('DB:ucla');
insert into db (name) values ('DB:SGD');

insert into db (name) values ('DB:PFAM');
insert into db (name) values ('DB:SUPERFAMILY');
insert into db (name) values ('DB:PROFILE');
insert into db (name) values ('DB:PRODOM');
insert into db (name) values ('DB:PRINTS');
insert into db (name) values ('DB:SMART');
insert into db (name) values ('DB:TIGRFAMs');
insert into db (name) values ('DB:PIR');

insert into db (name) values ('DB:Affymetrix_U133');
insert into db (name) values ('DB:Affymetrix_U133PLUS');
insert into db (name) values ('DB:Affymetrix_U95');
insert into db (name) values ('DB:LocusLink');
insert into db (name) values ('DB:RefSeq_protein');
insert into db (name) values ('DB:GenBank_protein');
insert into db (name) values ('DB:OMIM');
insert into db (name) values ('DB:Swiss');
insert into db (name) values ('DB:RefSNP');
insert into db (name) values ('DB:TSC');
--insert into db (name, contact_id, description, urlprefix) values ('DB:affy:U133',(select contact_id from contact where name = 'null'),'Affymetrix U133','http://https://www.affymetrix.com/analysis/netaffx/fullrecord.affx?pk=HG-U133_PLUS_2:');
--insert into db (name, contact_id, description, urlprefix) values ('DB:affy:U95',(select contact_id from contact where name = 'null'),'Affymetrix U95','http://https://www.affymetrix.com/analysis/netaffx/fullrecord.affx?pk=HG-U95AV2:');

insert into db (name, description) values ('DB:GR','Gramene');
insert into db (name, description, urlprefix) values ('DB:uniprot','UniProt/TrEMBL','http://us.expasy.org/cgi-bin/niceprot.pl?');
insert into db (name, description, urlprefix) values ('DB:refseq:mrna','RefSeq mRNA','http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=search&db=nucleotide&dopt=GenBank&term=');
insert into db (name, description, urlprefix) values ('DB:refseq:protein','RefSeq Protein','http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=search&db=protein&dopt=GenBank&term=');
insert into db (name, description, urlprefix) values ('DB:unigene','Unigene','http://www.ncbi.nih.gov/entrez/query.fcgi?db=unigene&cmd=search&term=');
insert into db (name, description, urlprefix) values ('DB:omim','OMIM','http://www.ncbi.nlm.nih.gov/entrez/dispomim.cgi?id=');
insert into db (name, description, urlprefix) values ('DB:locuslink','LocusLink','http://www.ncbi.nlm.nih.gov/LocusLink/LocRpt.cgi?l=');
insert into db (name, description, urlprefix) values ('DB:genbank:mrna','GenBank mRNA','http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=search&db=nucleotide&dopt=GenBank&term=');
insert into db (name, description, urlprefix) values ('DB:genbank:protein','GenBank Protein','http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=search&db=protein&dopt=GenBank&term=');
insert into db (name, description, urlprefix) values ('DB:swissprot:display','SwissProt','http://us.expasy.org/cgi-bin/niceprot.pl?');
insert into db (name, description, urlprefix) values ('DB:pfam','Pfam','http://www.sanger.ac.uk/cgi-bin/Pfam/dql.pl?query=');

insert into analysis (name,program,programversion) values ('dabg' ,'dabg' ,'dabg' );
insert into analysis (name,program,programversion) values ('dchip','dchip','dchip');
insert into analysis (name,program,programversion) values ('gcrma','gcrma','gcrma');
insert into analysis (name,program,programversion) values ('mas5' ,'mas5' ,'mas5' );
insert into analysis (name,program,programversion) values ('mpam' ,'mpam' ,'mpam' );
insert into analysis (name,program,programversion) values ('plier','plier','plier');
insert into analysis (name,program,programversion) values ('rma'  ,'rma'  ,'rma'  );
insert into analysis (name,program,programversion) values ('sea'  ,'sea'  ,'sea'  );
insert into analysis (name,program,programversion) values ('vsn'  ,'vsn'  ,'vsn'  );

insert into arraydesign (name,manufacturer_id,platformtype_id) values ('unknown'                                    , (select contact_id from contact where name = 'null'),(select cvterm_id from cvterm where name = 'null'));
insert into arraydesign (name,manufacturer_id,platformtype_id) values ('virtual array'                              , (select contact_id from contact where name = 'null'),(select cvterm_id from cvterm where name = 'null'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_HG-U133_Plus_2' , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_HG-U133A'       , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_HG-U133A_2'     , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_HG-U133B'       , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_HG-U95Av2'      , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_HG-U95B'        , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_HG-U95C'        , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_HG-U95D'        , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_HG-U95E'        , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_HuExon1'        , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_HuGeneFL'       , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_U74Av2'         , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_MG-U74Av2'      , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_MG-U74Bv2'      , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_MG-U74Cv2'      , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_RG-U34A'        , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_RG-U34B'        , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_RG-U34C'        , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_RT-U34'         , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_RN-U34'         , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_YG-S98'         , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_Yeast_2'        , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_RAE230A'        , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_RAE230B'        , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_Rat230_2'       , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_MOE430A'        , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_MOE430B'        , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_Mouse430_2'     , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_Mouse430A_2'    , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_ATH1-121501'    , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_Mapping100K_Hind240' , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_Mapping100K_Xba240'  , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_Mapping10K_Xba131'   , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_Mapping10K_Xba142'   , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_Mapping500K_NspI'    , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));
insert into arraydesign (name,manufacturer_id,platformtype_id,substratetype_id) values ('Affymetrix_Mapping500K_StyI'    , (select contact_id from contact where name = 'Affymetrix'),(select cvterm_id from cvterm where name = 'photochemical_oligo'),(select cvterm_id from cvterm where name = 'glass'));

insert into cv (name) values ('developmental stages');
insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'developmental stages:fetus');
insert into cvterm (name,cv_id,dbxref_id) values ('fetus',      (select cv_id from cv where name = 'local'),(select dbxref_id from dbxref where accession='developmental stages:fetus'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'developmental stages:neonate');
insert into cvterm (name,cv_id,dbxref_id) values ('neonate',    (select cv_id from cv where name = 'developmental stages'), (select dbxref_id from dbxref where accession='developmental stages:neonate'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'developmental stages:child');
insert into cvterm (name,cv_id,dbxref_id) values ('child',      (select cv_id from cv where name = 'developmental stages'), (select dbxref_id from dbxref where accession='developmental stages:child'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'developmental stages:adult_young');
insert into cvterm (name,cv_id,dbxref_id) values ('adult_young',(select cv_id from cv where name = 'developmental stages'),(select dbxref_id from dbxref where accession='developmental stages:adult_young'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'developmental stages:adult');
insert into cvterm (name,cv_id,dbxref_id) values ('adult',      (select cv_id from cv where name = 'developmental stages'),(select dbxref_id from dbxref where accession='developmental stages:adult'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'developmental stages:adult_old');
insert into cvterm (name,cv_id,dbxref_id) values ('adult_old',  (select cv_id from cv where name = 'developmental stages'), (select dbxref_id from dbxref where accession='developmental stages:adult_old'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'local:survival_time');
insert into cvterm (name,cv_id,dbxref_id) values ('survival_time',(select cv_id from cv where name = 'local'),(select dbxref_id from dbxref where accession='local:survival_time'));


insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'Statistical Terms:n');
insert into cvterm (name,definition,cv_id,dbxref_id) values ('n','sensu statistica',  (select cv_id from cv where name = 'Statistical Terms'),(select dbxref_id from dbxref where accession='Statistical Terms:n'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'Statistical Terms:minimum');
insert into cvterm (name,definition,cv_id,dbxref_id) values ('minimum','sensu statistica',  (select cv_id from cv where name = 'Statistical Terms'),(select dbxref_id from dbxref where accession='Statistical Terms:minimum'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'Statistical Terms:maximum');
insert into cvterm (name,definition,cv_id,dbxref_id) values ('maximum','sensu statistica',  (select cv_id from cv where name = 'Statistical Terms'),(select dbxref_id from dbxref where accession='Statistical Terms:maximum'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'Statistical Terms:modality');
insert into cvterm (name,definition,cv_id,dbxref_id) values ('modality','sensu statistica',  (select cv_id from cv where name = 'Statistical Terms'),(select dbxref_id from dbxref where accession='Statistical Terms:modality'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'Statistical Terms:modality p');
insert into cvterm (name,definition,cv_id,dbxref_id) values ('modality p','sensu statistica',  (select cv_id from cv where name = 'Statistical Terms'),(select dbxref_id from dbxref where accession='Statistical Terms:modality p'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'Statistical Terms:mean');
insert into cvterm (name,definition,cv_id,dbxref_id) values ('mean','sensu statistica',  (select cv_id from cv where name = 'Statistical Terms'),(select dbxref_id from dbxref where accession='Statistical Terms:mean'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'Statistical Terms:median');
insert into cvterm (name,definition,cv_id,dbxref_id) values ('median','sensu statistica',  (select cv_id from cv where name = 'Statistical Terms'),(select dbxref_id from dbxref where accession='Statistical Terms:median'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'Statistical Terms:mode');
insert into cvterm (name,definition,cv_id,dbxref_id) values ('mode','sensu statistica',  (select cv_id from cv where name = 'Statistical Terms'),(select dbxref_id from dbxref where accession='Statistical Terms:mode'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'Statistical Terms:quartile 1');
insert into cvterm (name,definition,cv_id,dbxref_id) values ('quartile 1','sensu statistica',  (select cv_id from cv where name = 'Statistical Terms'),(select dbxref_id from dbxref where accession='Statistical Terms:quartile 1'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'Statistical Terms:quartile 3');
insert into cvterm (name,definition,cv_id,dbxref_id) values ('quartile 3','sensu statistica',  (select cv_id from cv where name = 'Statistical Terms'),(select dbxref_id from dbxref where accession='Statistical Terms:quartile 3'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'Statistical Terms:skewness');
insert into cvterm (name,definition,cv_id,dbxref_id) values ('skewness','sensu statistica',  (select cv_id from cv where name = 'Statistical Terms'),(select dbxref_id from dbxref where accession='Statistical Terms:skewness'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'Statistical Terms:kurtosis');
insert into cvterm (name,definition,cv_id,dbxref_id) values ('kurtosis','sensu statistica',  (select cv_id from cv where name = 'Statistical Terms'),(select dbxref_id from dbxref where accession='Statistical Terms:kurtosis'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'Statistical Terms:chi square p');
insert into cvterm (name,definition,cv_id,dbxref_id) values ('chi square p','sensu statistica',  (select cv_id from cv where name = 'Statistical Terms'),(select dbxref_id from dbxref where accession='Statistical Terms:chi square p'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'Statistical Terms:standard deviation');
insert into cvterm (name,definition,cv_id,dbxref_id) values ('standard deviation','sensu statistica',  (select cv_id from cv where name = 'Statistical Terms'),(select dbxref_id from dbxref where accession='Statistical Terms:standard deviation'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'Statistical Terms:expectation maximization gaussian mean');
insert into cvterm (name,definition,cv_id,dbxref_id) values ('expectation maximization gaussian mean','sensu statistica',  (select cv_id from cv where name = 'Statistical Terms'),(select dbxref_id from dbxref where accession='Statistical Terms:expectation maximization gaussian mean'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'Statistical Terms:expectation maximization p');
insert into cvterm (name,definition,cv_id,dbxref_id) values ('expectation maximization p','sensu statistica',  (select cv_id from cv where name = 'Statistical Terms'),(select dbxref_id from dbxref where accession='Statistical Terms:expectation maximization p'));

insert into dbxref (db_id,accession) values ((select db_id from db where name='null'), 'Statistical Terms:histogram');
insert into cvterm (name,definition,cv_id,dbxref_id) values ('histogram','sensu statistica',  (select cv_id from cv where name = 'Statistical Terms'),(select dbxref_id from dbxref where accession='Statistical Terms:histogram'));

insert into cv (name,definition) values ('autocreated','Terms that are automatically inserted by loading software');


--this table will probably end up in general.sql
 CREATE TABLE public.materialized_view   (       
                                materialized_view_id SERIAL,
                                last_update TIMESTAMP,
                                refresh_time INT,
                                name VARCHAR(64) UNIQUE,
                                mv_schema VARCHAR(64),
                                mv_table VARCHAR(128),
                                mv_specs TEXT,
                                indexed TEXT,
                                query TEXT,
                                special_index TEXT
                                );

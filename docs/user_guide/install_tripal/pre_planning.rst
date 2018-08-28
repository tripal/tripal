
Pre-planning
============

IT Infrastructure
-----------------
Tripal requires a server with adequate resources to handle the expected load and systems administration skills to get the machine up and running, applications installed and everything properly secure. Tripal requires a PostgreSQL databases server, Apache (or equivalent) web server, PHP5 and several configuration options to make it all work. However, once these prerequisites are met, working with Drupal and Tripal are quite easy.

There are several ways you could setup a Tripal site:

- **Option #1** In-house dedicated servers: You may have access to servers in your own department or group which you have administrative control and wish to install Tripal on these.
- **Option #2** Institutional IT support: Your institution may provide IT servers and would support your efforts to install a website with database backend.
- **Option #3** Commercial web-hosting: If options #1 and #2 are not available to you, commercial web-hosting is an affordable option. For large databases you may require a dedicated server. Bluehost.com is a web hosting service that provides hosting compatible with Drupal, Tripal and its dependencies.
- Option #4 In the Cloud: Tripal is a part of the GMOD in the cloud Amazon AWS image created by GMOD. It is also accompanied by other GMOD tools such as GBrowse2, JBrowse, Apollo and WebApollo.

After selection of one of the options above you can arrange your database/webserver in the following ways:

- **Arrangement #1**: The database and web server are housed on a single server. This is the approach taken by this course. It is necessary to gain access to a machine with enough memory (RAM), hard disk speed and space, and processor power to handle both services.
- **Arrangement #2**: The database and web server are housed on different servers. This provides dedicated resources to each service (i.e. web and database).

Selection of an appropriate machine

Databases are typically bottle-necked by RAM and disk speed. Selection of the correct balance of RAM, disk speed, disk size and CPU speed is important and dependent on the size of the data. The best advice is to consult an IT professional who can recommend a server installation tailored for the expected size of your data.

.. note::

  Tripal does require command-line access to the web server with adequate local file storage for loading of large data files. Be sure to check with your service provider to make sure command-line access is possible.

Technical Skills
----------------
Depending on your needs, you may need additional Technical support. Use the following questions to help determine those needs:

**Tripal already supports my data, what personnel do I need to maintain it?**

Someone to install/setup the IT infrastructure
Someone who understands the data to load it properly

**Tripal does not yet support all of my data, but I want to use what's been done and expand on it....?**

Someone to install/setup the IT infrastructure
Someone who understands the data to load it properly
PHP/HTML/CSS/JavaScript programmer(s) to write your custom extensions


Development and Production Instances
------------------------------------
It is recommended that you have separate development and production instances of Tripal. The staging or development instance allows you to test new functionality, add customization, or test modification or additions to data without disturbing the production instance. The production instance serves content to the rest of the world. Once you are certain that customizations and new functionality will work well on the development instance you can easily re-implement or copy these over to the production site. Sometimes it may take a few trials to load data in the way you want. A development sites lets you take time to test data loading prior to making it public. The development site can be password-protected to only allow access to site administrators, developers or collaborators.

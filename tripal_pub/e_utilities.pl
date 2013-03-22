#!/usr/bin/perl -w
# ---------------------------------------------------------------------------
# Define library for the 'get' function used in the next section.
# $utils contains route for the utilities.
# $db, $query, and $report may be supplied by the user when prompted; 
# if not answered, default values, will be assigned as shown below.

use LWP::Simple;
use utf8;

my $utils = "http://www.ncbi.nlm.nih.gov/entrez/eutils";

my $db     = "Pubmed";
my $query  = $ARGV[0];
my $report = $ARGV[1];

# ---------------------------------------------------------------------------
# $esearch contÁins the PATH & parameters for the ESearch call
# $esearch_result containts the result of the ESearch call
# the results are displayed Ánd parsed into variables 
# $Count, $QueryKey, and $WebEnv for later use and then displayed.

my $esearch = "$utils/esearch.fcgi?" .
              "db=$db&retmax=1&usehistory=y&term=";

my $esearch_result = get($esearch . $query);



$esearch_result =~ 
  m|<Count>(\d+)</Count>.*<QueryKey>(\d+)</QueryKey>.*<WebEnv>(\S+)</WebEnv>|s;

my $Count    = $1;
my $QueryKey = $2;
my $WebEnv   = $3;

# ---------------------------------------------------------------------------
# this area defines a loop which will display $retmax citation results from 
# Efetch each time the the Enter Key is pressed, after a prompt.

my $retstart;
my $retmax=3;

for($retstart = 0; $retstart < $Count; $retstart += $retmax) {
  my $efetch = "$utils/efetch.fcgi?" .
               "rettype=$report&retmode=text&retstart=$retstart&retmax=$retmax&" .
               "db=$db&query_key=$QueryKey&WebEnv=$WebEnv";
	
  #print "\nEF_QUERY=$efetch\n";     


  my $efetch_result = get($efetch);

	#open( $fh, '>', \$efetch_result);

  
  print $efetch_result;
    
  #print binmode($fh, ":utf8");
  
}

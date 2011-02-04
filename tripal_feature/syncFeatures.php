<?php


# This script can be run as a stand-alone script to sync all the features from chado to drupal
// Parameter f specifies the feature_id to sync
// -f 0 will sync all features 

$arguments = getopt("f:");

if(isset($arguments['f'])){
   $drupal_base_url = parse_url('http://www.example.com');
   $_SERVER['HTTP_HOST'] = $drupal_base_url['host'];
   $_SERVER['REQUEST_URI'] = $_SERVER['SCRIPT_NAME'] = $_SERVER['PHP_SELF'];
   $_SERVER['REMOTE_ADDR'] = NULL;
   $_SERVER['REQUEST_METHOD'] = NULL;
	
   require_once 'includes/bootstrap.inc';
   drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

   $feature_id = $arguments['f'];

   if($feature_id > 0 ){ 
      print "syncing feature $feature_id\n";
      tripal_feature_sync_feature($feature_id); 
   }
   else{ 
      print "syncing all features...\n";
      tripal_feature_sync_features();
   }   
}
   

/************************************************************************
 *
 */
function tripal_feature_sync_features ($max_sync = 0, $job_id = NULL){
   //print "Syncing features (max of $max_sync)\n";
   $i = 0;

   // get the list of available sequence ontology terms for which
   // we will build drupal pages from features in chado.  If a feature
   // is not one of the specified typse we won't build a node for it.
   $allowed_types = variable_get('chado_feature_types','EST contig');
   $allowed_types = preg_replace("/[\s\n\r]+/"," ",$allowed_types);
   $so_terms = split(' ',$allowed_types);
   $where_cvt = "";
   foreach ($so_terms as $term){
      $where_cvt .= "CVT.name = '$term' OR ";
   }
   $where_cvt = substr($where_cvt,0,strlen($where_cvt)-3);  # strip trailing 'OR'

   // get the list of organisms that are synced and only include features from
   // those organisms
   $orgs = organism_get_synced();
   $where_org = "";
   foreach($orgs as $org){
      $where_org .= "F.organism_id = $org->organism_id OR ";
   }
   $where_org = substr($where_org,0,strlen($where_org)-3);  # strip trailing 'OR'

   // use this SQL statement to get the features that we're going to upload
   $sql = "SELECT feature_id ".
          "FROM {FEATURE} F ".
          "   INNER JOIN Cvterm CVT ON F.type_id = CVT.cvterm_id ".
          "WHERE ($where_cvt) AND ($where_org) ".
          "ORDER BY feature_id";
   // get the list of features
   $previous_db = tripal_db_set_active('chado');  // use chado database
   $results = db_query($sql);

   tripal_db_set_active($previous_db);  // now use drupal database

   // load into ids array
   $count = 0;
   $ids = array();
   while($id = db_fetch_object($results)){
      $ids[$count] = $id->feature_id;
      $count++;
   }

   // make sure our vocabularies are set before proceeding
   tripal_feature_set_vocabulary();

   // pre-create the SQL statement that will be used to check
   // if a feature has already been synced.  We skip features
   // that have been synced
   $sql = "SELECT * FROM {chado_feature} WHERE feature_id = %d";

   // Iterate through features that need to be synced
   $interval = intval($count * 0.01);
   foreach($ids as $feature_id){
      // update the job status every 1% features
      if($job_id and $i % $interval == 0){
         tripal_job_set_progress($job_id,intval(($i/$count)*100));
      }
      // if we have a maximum number to sync then stop when we get there
      // if not then just continue on
      if($max_sync and $i == $max_sync){
         return '';
      }
      if(!db_fetch_object(db_query($sql,$feature_id))){
        
         # parsing all the features can cause memory overruns 
         # we are not sure why PHP does not clean up the memory as it goes
         # to avoid this problem we will call this script through an
         # independent system call

         $cmd = "php " . drupal_get_path('module', 'tripal_feature') . "/syncFeatures.php -f $feature_id ";
         system($cmd);

      }
      $i++;
   }

   return '';
}

function tripal_feature_sync_feature ($feature_id){
//   print "\tfeature $feature_id\n";

   $mem = memory_get_usage(TRUE);
   $mb = $mem/1048576;
//   print "$mb mb\n";

   global $user;
   $create_node = 1;   // set to 0 if the node exists and we just sync and not create

   // get the accession prefix
   $aprefix = variable_get('chado_feature_accession_prefix','ID');

   // if we don't have a feature_id then return
   if(!$feature_id){
      drupal_set_message(t("Please provide a feature_id to sync"));
      return '';
   }

   // get information about this feature
   $fsql = "SELECT F.feature_id, F.name, F.uniquename,O.genus, ".
           "    O.species,CVT.name as cvname,F.residues,F.organism_id ".
           "FROM {FEATURE} F ".
           "  INNER JOIN Cvterm CVT ON F.type_id = CVT.cvterm_id ".
           "  INNER JOIN Organism O ON F.organism_id = O.organism_ID ".
           "WHERE F.feature_id = %d";
   $previous_db = tripal_db_set_active('chado');  // use chado database
   $feature = db_fetch_object(db_query($fsql,$feature_id));
   tripal_db_set_active($previous_db);  // now use drupal database

   // check to make sure that we don't have any nodes with this feature name as a title
   // but without a corresponding entry in the chado_feature table if so then we want to
   // clean up that node.  (If a node is found we don't know if it belongs to our feature or
   // not since features can have the same name/title.)
   $tsql =  "SELECT * FROM {node} N ".
            "WHERE title = '%s'";
   $cnsql = "SELECT * FROM {chado_feature} ".
            "WHERE nid = %d";
   $nodes = db_query($tsql,$feature->name);
   // cycle through all nodes that may have this title
   while($node = db_fetch_object($nodes)){
      $feature_nid = db_fetch_object(db_query($cnsql,$node->nid));
      if(!$feature_nid){
         drupal_set_message(t("$feature_id: A node is present but the chado_feature entry is missing... correcting"));
         node_delete($node->nid);
      }
   }

   // check if this feature already exists in the chado_feature table.
   // if we have a chado feature, we want to check to see if we have a node
   $cfsql = "SELECT * FROM {chado_feature} ".
            "WHERE feature_id = %d";
   $nsql =  "SELECT * FROM {node} ".
            "WHERE nid = %d";
   $chado_feature = db_fetch_object(db_query($cfsql,$feature->feature_id));
   if($chado_feature){
      drupal_set_message(t("$feature_id: A chado_feature entry exists"));
      $node = db_fetch_object(db_query($nsql,$chado_feature->nid));
      if(!$node){
         // if we have a chado_feature but not a node then we have a problem and
         // need to cleanup
         drupal_set_message(t("$feature_id: The node is missing, but has a chado_feature entry... correcting"));
         $df_sql = "DELETE FROM {chado_feature} WHERE feature_id = %d";
         db_query($df_sql,$feature_id);
      } else {
         drupal_set_message(t("$feature_id: A corresponding node exists"));
         $create_node = 0;
      }
   }

   // if we've encountered an error then just return.
   if($error_msg = db_error()){
      //print "$error_msg\n";
      return '';
   }

   // if a drupal node does not exist for this feature then we want to
   // create one.  Note that the node_save call in this block
   // will call the hook_submit function which
   if($create_node){
      // get the organism for this feature
      $sql = "SELECT * FROM {organism} WHERE organism_id = %d";
      $organism = db_fetch_object(db_query($sql,$feature->organism_id));

      drupal_set_message(t("$feature_id: Creating node $feature->name"));
      $new_node = new stdClass();
      $new_node->type = 'chado_feature';
      $new_node->uid = $user->uid;
      $new_node->title = "$feature->uniquename ($feature->cvname) $organism->genus $organism->species";
      $new_node->name = "$feature->name";
      $new_node->uniquename = "$feature->uniquename";
      $new_node->feature_id = $feature->feature_id;
      $new_node->residues = $feature->residues;
      $new_node->organism_id = $feature->organism_id;
      $new_node->feature_type = $feature->cvname;

      // validate the node and if okay then submit
      node_validate($new_node);
      if ($errors = form_get_errors()) {
         foreach($errors as $key => $msg){
            drupal_set_message($msg);
         }
         return $errors;
      } else {
         $node = node_submit($new_node);
         node_save($node);
      }

   }
   else {
      $node = $chado_feature;
   }


   // set the taxonomy for this node
   drupal_set_message(t("$feature_id ($node->nid): setting taxonomy"));
   tripal_feature_set_taxonomy($node,$feature_id);

   // reindex the node
   // drupal_set_message(t("$feature_id( $node->nid): indexing"));
   // tripal_feature_index_feature ($feature_id,$node->nid);

   // remove any URL alias that may already exist and recreate
   drupal_set_message(t("$feature_id ($node->nid): setting URL alias"));
   db_query("DELETE FROM {url_alias} WHERE dst = '%s'", "$aprefix$feature_id");
   path_set_alias("node/$node->nid","$aprefix$feature_id");

   return '';
}



/*******************************************************************************
 *  Returns a list of organisms that are currently synced with Drupal
 */
function organism_get_synced() {

   // use this SQL for getting synced organisms
   $dsql =  "SELECT * FROM {chado_organism}";
   $orgs = db_query($dsql);

   // use this SQL statement for getting the organisms
   $csql =  "SELECT * FROM {Organism} ".
            "WHERE organism_id = %d";

   $org_list = array();

   // iterate through the organisms and build an array of those that are synced
   while($org = db_fetch_object($orgs)){
      $previous_db = tripal_db_set_active('chado');  // use chado database
      $info = db_fetch_object(db_query($csql,$org->organism_id));
      tripal_db_set_active($previous_db);  // now use drupal database
      $org_list[] = $info;
   }    
   return $org_list;
}





?>

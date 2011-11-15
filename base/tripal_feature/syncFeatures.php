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
      tripal_feature_sync_feature($feature_id); 
   }
   else{ 
      print "syncing all features...\n";
      tripal_feature_sync_features();
   }   
}
/**
*
*/
function tripal_feature_sync_form (){

   $form['description'] = array(
      '#type' => 'item',
      '#value' => t("Add feature types, optionally select an organism and ".
         "click the 'Sync all Features' button to create Drupal ".
         "content for features in chado. Only features of the types listed ".
         "below in the Feature Types box will be synced. You may limit the ".
         "features to be synced by a specific organism. Depending on the ".
         "number of features in the chado database this may take a long ".
         "time to complete. "),
   );

   $form['feature_types'] = array(
      '#title'       => t('Feature Types'),
      '#type'        => 'textarea',
      '#description' => t('Enter the names of the sequence types that the ".
         "site will support with independent pages.  Pages for these data ".
         "types will be built automatically for features that exist in the ".
         "chado database.  The names listed here should be spearated by ".
         "spaces or entered separately on new lines. The names must match ".
         "exactly (spelling and case) with terms in the sequence ontology'),
      '#required'    => TRUE,
      '#default_value' => variable_get('chado_sync_feature_types','gene contig'),
   );

   // get the list of organisms
   $sql = "SELECT * FROM {organism} ORDER BY genus, species";
   $orgs = tripal_organism_get_synced(); 
   $organisms[] = ''; 
   foreach($orgs as $organism){
      $organisms[$organism->organism_id] = "$organism->genus $organism->species ($organism->common_name)";
   }
   $form['organism_id'] = array (
     '#title'       => t('Organism'),
     '#type'        => t('select'),
     '#description' => t("Choose the organism for which features will be deleted."),
     '#options'     => $organisms,
   );


   $form['button'] = array(
      '#type' => 'submit',
      '#value' => t('Sync all Features'),
      '#weight' => 3,
   );

   return $form;
}
/**
*
*/
function tripal_feature_sync_form_validate ($form, &$form_state){
   $organism_id   = $form_state['values']['organism_id'];
   $feature_types = $form_state['values']['feature_types'];

   // nothing to do
}
/**
*
*/
function tripal_feature_sync_form_submit ($form, &$form_state){

   global $user;

   $organism_id   = $form_state['values']['organism_id'];
   $feature_types = $form_state['values']['feature_types'];

   $job_args = array(0,$organism_id,$feature_types);

   if($organism_id){
      $organism = tripal_core_chado_select('organism',array('genus','species'),array('organism_id' => $organism_id));
      $title = "Sync all features for " .  $organism[0]->genus . " " . $organism[0]->species;
   } else {
      $title = t('Sync all features for all synced organisms');
   }

   variable_set('chado_sync_feature_types',$feature_types);

   tripal_add_job($title,'tripal_feature',
         'tripal_feature_sync_features',$job_args,$user->uid);
}
/**
*
*/   
function tripal_feature_set_urls($job_id = NULL){
   // first get the list of features that have been synced
   $sql = "SELECT * FROM {chado_feature}";
   $nodes = db_query($sql);
   while($node = db_fetch_object($nodes)){
      // now get the feature details
      $feature_arr = tripal_core_chado_select('feature',
         array('feature_id','name','uniquename'),
         array('feature_id' => $node->feature_id));
      $feature = $feature_arr[0];

      tripal_feature_set_feature_url($node,$feature);
   }
}
/**
*
*/
function tripal_feature_set_feature_url($node,$feature){

   // determine which URL alias to use
   $alias_type = variable_get('chado_feature_url','internal ID');
   $aprefix = variable_get('chado_feature_accession_prefix','ID');
   switch ($alias_type) {
      case 'feature name':
         $url_alias = $feature->name;
         break;
      case 'feature unique name':
         $url_alias = $feature->uniquename;
         break;
      default:
         $url_alias = "$aprefix$feature->feature_id";
   }
   print "Setting $alias_type as URL alias for $feature->name: node/$node->nid => $url_alias\n";
   // remove any previous alias
   db_query("DELETE FROM {url_alias} WHERE src = '%s'", "node/$node->nid");
   // add the new alias
   path_set_alias("node/$node->nid",$url_alias);
}
/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_sync_features ($max_sync = 0, $organism_id = NULL, 
   $feature_types = NULL, $job_id = NULL)
{
   //print "Syncing features (max of $max_sync)\n";
   $i = 0;

   // get the list of available sequence ontology terms for which
   // we will build drupal pages from features in chado.  If a feature
   // is not one of the specified typse we won't build a node for it.
   if(!$feature_types){
      $allowed_types = variable_get('chado_sync_feature_types','gene contig');
   } else {
      $allowed_types = $feature_types;
   }
   $allowed_types = preg_replace("/[\s\n\r]+/"," ",$allowed_types);

   print "Looking for features of type: $allowed_types\n";

   $so_terms = split(' ',$allowed_types);
   $where_cvt = "";
   foreach ($so_terms as $term){
      $where_cvt .= "CVT.name = '$term' OR ";
   }
   $where_cvt = substr($where_cvt,0,strlen($where_cvt)-3);  # strip trailing 'OR'

   // get the list of organisms that are synced and only include features from
   // those organisms
   $orgs = tripal_organism_get_synced();
   $where_org = "";
   foreach($orgs as $org){
      if($organism_id){
         if($org->organism_id and $org->organism_id == $organism_id){
            $where_org .= "F.organism_id = $org->organism_id OR ";
         }
      } 
      else {
         if($org->organism_id){
            $where_org .= "F.organism_id = $org->organism_id OR ";
         }
      }
   }
   $where_org = substr($where_org,0,strlen($where_org)-3);  # strip trailing 'OR'

   // use this SQL statement to get the features that we're going to upload
   $sql = "SELECT feature_id ".
          "FROM {FEATURE} F ".
          "  INNER JOIN Cvterm CVT ON F.type_id = CVT.cvterm_id ".
          "  INNER JOIN CV on CV.cv_id = CVT.cv_id ".
          "WHERE ($where_cvt) AND ($where_org) AND CV.name = 'sequence' ".
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
   if($interval > 1){
      $interval = 1;
   }
   $num_ids = sizeof($ids);
   $i = 0;
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
         print "$i of $num_ids Syncing feature id: $feature_id\n";
         $cmd = "php " . drupal_get_path('module', 'tripal_feature') . "/syncFeatures.php -f $feature_id ";
         system($cmd);

      }
      $i++;
   }

   return '';
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_sync_feature ($feature_id){
//   print "\tSyncing feature $feature_id\n";

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

   // get the synonyms for this feature
   $synsql = "SELECT S.name ".
             "FROM {feature_synonym} FS ".
             "  INNER JOIN {synonym} S on FS.synonym_id = S.synonym_id ".
             "WHERE FS.feature_id = %d";
   $previous_db = tripal_db_set_active('chado');  // use chado database
   $synonyms = db_query($synsql,$feature_id);
   tripal_db_set_active($previous_db);  // now use drupal database

   // now add these synonyms to the feature object as a single string   
   $synstring = '';
   while($synonym = db_fetch_object($synonyms)){
      $synstring .= "$synonym->name\n";
   }        
   $feature->synonyms = $synstring;

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
      $new_node->title = "$feature->name, $feature->uniquename ($feature->cvname) $organism->genus $organism->species";
      $new_node->fname = "$feature->name";
      $new_node->uniquename = "$feature->uniquename";
      $new_node->feature_id = $feature->feature_id;
      $new_node->residues = $feature->residues;
      $new_node->organism_id = $feature->organism_id;
      $new_node->feature_type = $feature->cvname;
      $new_node->synonyms = $feature->synonyms;

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

   // set the URL alias for this node
   tripal_feature_set_feature_url($node,$feature);


   return '';
}
?>

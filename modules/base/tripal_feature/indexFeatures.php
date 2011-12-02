<?php

// This script can be run as a stand-alone script to sync all the features from chado to drupal
//
// To index a single feature
// -i feature_id
// -n node_id 
//
// To index all features
// -i 0 

$arguments = getopt("i:n:");

if(isset($arguments['i'])){
   $drupal_base_url = parse_url('http://www.example.com');
   $_SERVER['HTTP_HOST'] = $drupal_base_url['host'];
   $_SERVER['REQUEST_URI'] = $_SERVER['SCRIPT_NAME'] = $_SERVER['PHP_SELF'];
   $_SERVER['REMOTE_ADDR'] = NULL;
   $_SERVER['REQUEST_METHOD'] = NULL;
	
   require_once 'includes/bootstrap.inc';
   drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

   $feature_id = $arguments['i'];
   $nid        = $arguments['n'];

   # print "\n";
   # print "feature id is $feature_id\n";
   # print "nid is $nid\n";
   # print "\n";

   if($feature_id > 0){ 
      # print "indexing feature $feature_id\n";
     // We register a shutdown function to ensure that the nodes
     // that are indexed will have proper entries in the search_totals
     // table.  Without these entries, the searching doesn't work
     // properly. This function may run for quite a while since
     // it must calculate the sum of the scores of all entries in
     // the search_index table.  In the case of common words like
     // 'contig', this will take quite a while
      register_shutdown_function('search_update_totals');
      tripal_feature_index_feature($feature_id, $nid); 
   }
   else{ 
      print "indexing all features...\n";
      tripal_features_reindex(0);
   }

}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_features_reindex ($max_sync,$job_id = NULL){
   $i = 0;

   // We register a shutdown function to ensure that the nodes
   // that are indexed will have proper entries in the search_totals
   // table.  Without these entries, the searching doesn't work
   // properly. This function may run for quite a while since
   // it must calculate the sum of the scores of all entries in
   // the search_index table.  In the case of common words like
   // 'contig', this will take quite a while
   register_shutdown_function('search_update_totals');

   // use this SQL statement to get the features that we're going to index. This
   // SQL statement is derived from the hook_search function in the Drupal API.
   // Essentially, this is the SQL statement that finds all nodes that need
   // reindexing, but adjusted to include the chado_feature
   $sql = "SELECT N.nid, N.title, CF.feature_id ".
          "FROM {node} N ".
          "  INNER JOIN chado_feature CF ON CF.nid = N.nid ";
   $results = db_query($sql);

   // load into ids array
   $count = 0;
   $chado_features = array();
   while($chado_feature = db_fetch_object($results)){
      $chado_features[$count] = $chado_feature;
      $count++;
   }

   // Iterate through features that need to be indexed 
   $interval = intval($count * 0.01);
   if($interval >= 0){
      $interval = 1;
   }
   foreach($chado_features as $chado_feature){

      // update the job status every 1% features
      if($job_id and $i % $interval == 0){
         $prog = intval(($i/$count)*100);
         tripal_job_set_progress($job_id,$prog);
         print "$prog%\n";
      }

      // sync only the max requested
      if($max_sync and $i == $max_sync){
         return '';
      }
      $i++;

      # tripal_feature_index_feature ($chado_feature->feature_id,$chado_feature->nid);
      # parsing all the features can cause memory overruns 
      # we are not sure why PHP does not clean up the memory as it goes
      # to avoid this problem we will call this script through an
      # independent system call

      $cmd = "php " . drupal_get_path('module', 'tripal_feature') . "/indexFeatures.php ";
      $cmd .= "-i $chado_feature->feature_id -n $chado_feature->nid ";

      # print "\t$cmd\n";
      # print "\tfeature id is $chado_feature->feature_id\n";
      # print "\tnid is $chado_feature->nid\n";
      # print "\n";

      system($cmd);
   }

   return '';
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_index_feature ($feature_id,$nid){
   #print "\tfeature $feature_id nid $nid\n";
   // return if we haven't been provided with a feature_id
   if(!$feature_id){
      return 0;
   }

   // if we only have a feature_id then let's find a corresponding
   // node.  If we can't find a node then return.
   if(!$nid){
      $nsql = "SELECT N.nid,N.title FROM {chado_feature} CF ".
              "  INNER JOIN {node} N ON N.nid = CF.nid ".
              "WHERE CF.feature_id = %d";
      $node = db_fetch_object(db_query($nsql,$feature_id));
      if(!$node){
         return 0;
      }
      $node = node_load($node->nid);
   } else {
      $node = node_load($nid);
   }

   // node load the noad, the comments and the taxonomy and
   // index
   $node->build_mode = NODE_BUILD_SEARCH_INDEX;
   $node = node_build_content($node, FALSE, FALSE);
   $node->body = drupal_render($node->content);
   node_invoke_nodeapi($node, 'view', FALSE, FALSE);
//   $node->body .= module_invoke('comment', 'nodeapi', $node, 'update index');
//   $node->body .= module_invoke('taxonomy','nodeapi', $node, 'update index');
   //   print "$node->title: $node->body\n";
   search_index($node->nid,'node',$node->body);

   # $mem = memory_get_usage(TRUE);
   # $mb = $mem/1048576;
   # print "$mb mb\n";

   return 1;
}

?>

<?php
// this template does not follow the typical Tripal API. Normally
// variables are expanded using the tripal_core_expand_chado_vars API
// function call, but expanding the relationships table does not yeild
// a meaningful order to the data.  Therefore, relationships are preprocessed
// into an array named 'all_relationships', which is used in the template below.

$pub = $variables['node']->pub;

$all_relationships = $pub->all_relationships;
$object_rels = $all_relationships['object'];
$subject_rels = $all_relationships['subject'];

// make the pub type a bit more human readable
$pub_type =  preg_replace("/_/", ' ', $pub->type_id->name);

if (count($object_rels) > 0 or count($subject_rels) > 0) {
?>
  <div id="tripal_pub-relationships-box" class="tripal_pub-info-box tripal-info-box">
    <div class="tripal_pub-info-box-title tripal-info-box-title">Relationships</div>
    <!--  <div class="tripal_pub-info-box-desc tripal-info-box-desc"></div> --><?php
    
      // first add in the subject relationships.  
      foreach ($subject_rels as $rel_type => $rels){
         // make the type a bit more human readable
         $rel_type = preg_replace("/_/", ' ', $rel_type);
         $rel_type = preg_replace("/^is/", '', $rel_type);
         // iterate through each parent   
         foreach ($rels as $obj_type => $objects){?>
           <p>This pub is a <b><?php print $rel_type ?></b> of the following pub(s):
           <table id="tripal_pub-relationships_as_object-table" class="tripal_pub-table tripal-table tripal-table-horz">
             <tr>
               <th>pub Name</th>
             </tr> <?php
             foreach ($objects as $object){ ?>
               <tr>
                 <td><?php 
                    if ($object->nid) {
                      print "<a href=\"" . url("node/" . $object->nid) . "\" target=\"_blank\">" . $object->title . "</a>";
                    }
                    else {
                      print $object->name;
                    } ?>
                 </td>
               </tr> <?php
             } ?>
             </table>
             </p><br><?php
         }
      }
      
      // second add in the object relationships.  
      foreach ($object_rels as $rel_type => $rels){
         // make the type more human readable
         $rel_type = preg_replace('/_/', ' ', $rel_type);
         $rel_type = preg_replace("/^is/", '', $rel_type);
         // iterate through the children         
         foreach ($rels as $subject_type => $subjects){?>
           <p>The following pubs are a <b><?php print $rel_type ?></b> of this pub:
           <table id="tripal_pub-relationships_as_object-table" class="tripal_pub-table tripal-table tripal-table-horz">
             <tr>
               <th>Title</th>
             </tr> <?php
             foreach ($subjects as $subject){ ?>
               <tr>
                 <td><?php 
                    if ($subject->nid) {
                      print "<a href=\"" . url("node/" . $subject->nid) . "\" target=\"_blank\">" . $subject->title . "</a>";
                    }
                    else {
                      print $subject->name;
                    } ?>
                 </td>
               </tr> <?php
             } ?>
             </table>
             </p><br><?php
         }
      } ?>
  </div> <?php
}

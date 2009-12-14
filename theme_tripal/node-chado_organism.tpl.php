<?php
//
// Copyright 2009 Clemson University
//
?>

<?php if($teaser){ ?>
    <a href="<?php print $node_url?>"><?php print $title?></a>
<?php } else { ?>
   <?php $features  = $node->features ?>
   <?php $libraries = $node->libraries ?>
   <div class="node<?php if ($sticky) { print " sticky"; } ?>
                   <?php if (!$status) { print " node-unpublished"; } ?>">
   <?php if ($picture) { print $picture; }?>
   <?php if ($page == 0) { ?>
     <h2 class="title"><a href="<?php print $node_url?>"><?php print $title?></a></h2>
   <?php }; ?>
   <span class="taxonomy"><?php print $terms?></span>
   <div class="content">
     <div class="tripal_organism-image">
        <img src=<?php print file_create_url(file_directory_path() . "/tripal/tripal_organism/images/".$node->genus."_".$node->species.".jpg")?>>
     </div>
     <?php if ($submitted): ?>     
        <div class="metanode"><p><?php print t('') .'<span class="author">'. theme('username', $node).'</span>' . t(' - Posted on ') . '<span class="date">'.format_date($node->created, 'custom', "d F Y").'</span>'; ?></p></div>
     <?php endif; ?>
     <div class="tripal_organism-details">
        <h3>Details</h3>
           <table class="tripal_table_vert">
              <tr>
                 <th nowrap>Common Name</th>
                 <td><?php print $node->common_name?></td>
              </tr>
              <tr>
                 <th>Genus</th>
                 <td><?php print $node->genus?></td>
              </tr>
              <tr>
                 <th>Species</th>
                 <td><?php print $node->species?></td>
              </tr>
           </table>
           <?php if($node->description){ ?>
              <b>Description</b>
              <p class="organism"><?php print $node->description?></p>
           <?php }; ?>
        </div>
     </div>
     <div class="content"><?php print $content ?></div>
     <?php if ($links) { ?>
       <div class="links"> <?php print $links?></div>
     <?php }; ?>
  </div>
<?php }; ?>


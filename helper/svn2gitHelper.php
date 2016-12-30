<?php

$lastline = exec('git branch -a', $data, $code);

foreach($data as $line) 
{
    $tmp = stristr($line, '/');
    $tmp = ltrim($tmp, '/');

    if (preg_match('/trunk/i', $tmp)) {
        echo 'ignore: trunk ';
        continue;
    }

    exec('git checkout '. $tmp);
    
    if (preg_match('/tags/i', $tmp)) {
        
        $tmp = strrchr($tmp, '/');
        $tmp = trim($tmp, '/');
    } else {
        $tmp = stristr($tmp, '/');
        $tmp = ltrim($tmp, '/');
    }
    
    
    exec('git tag '. $tmp);
    
    exec('git checkout master');
    
    echo $tmp . PHP_EOL;
}



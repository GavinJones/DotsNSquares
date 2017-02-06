module.exports = [
  { 
    "type": "heading", 
    "defaultValue": "Dots and Squares" 
  }, 
  { 
    "type": "text", 
    "defaultValue": "By Gavin Jones." 
  },
  { "type": "section",
    "items": [
    {
      "type": "heading",
      "defaultValue": "General"
    },  
    {
      "type": "input",
      "messageKey": "reminder",
      "label": "Reminder (Pebble 2 only)"
    },      
    {
     "type": "slider",
     "messageKey": "stepgoal",
     "defaultValue": 8000,
     "label": "Step Goal",
     "description": "Choose required step goal",
     "min": 2000,
     "max": 20000,
     "step": 1000
   },
   {
     "type": "toggle",
     "messageKey": "buzzondisconnect",
         "label": "Buzz on disconnect"
   },
   {
      "type": "color",
      "messageKey": "facecolour",
      "defaultValue": "000000",
      "label": "Divider Colour",
      "sunlight": false,
      "allowGray": true
    }    
   ]
},   
{ "type": "section",
  "items": [
    {
      "type": "heading",
      "defaultValue": "Time"
    },  
    {
      "type": "color",
      "messageKey": "timecolour",
      "defaultValue": "0000ff",
      "label": "Background Colour",
      "sunlight": false,
      "allowGray": true
    },
    {
      "type": "color",
      "messageKey": "timetextcolour",
      "defaultValue": "ffffff",
      "label": "Text Colour",
      "sunlight": false,
      "allowGray": true
    }
    ]
},    
{ "type": "section",
  "items": [
    {
      "type": "heading",
      "defaultValue": "Date"
    },  
   {
     "type": "color",
     "messageKey": "datecolour",
     "defaultValue": "0055ff",
     "label": "Background Colour",
     "sunlight": false,
     "allowGray": true
   },
   {
     "type": "color",
     "messageKey": "datetextcolour",
     "defaultValue": "ffffff",
     "label": "Text Colour",
     "sunlight": false,
     "allowGray": true
   }
   ]
},    
{
  "type": "submit",
  "defaultValue": "Save"
}  
  
];


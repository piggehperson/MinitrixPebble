module.exports = [
  {
    "type": "heading",
    "defaultValue": "Minitrix Configuration"
  },
  {
    "type": "text",
    "defaultValue": "Recalibrate your Omnitrix device."
  },
  
  {
    "type": "section",
    "items": [
      {
      "type": "select",
      "messageKey": "TimeDisplayMode",
      "defaultValue": "digital",
      "label": "Time display mode",
      "options": [
        { 
          "label": "Analog", 
          "value": "analog" 
        },
        { 
          "label": "Digital",
          "value": "digital" 
        },
      ]
    },
      
      {
      "type": "select",
      "messageKey": "WatchDialStyle",
      "defaultValue": "omniverse",
      "label": "Omnitrix style",
      "options": [
        { 
          "label": "Classic", 
          "value": "classic" 
        },
        { 
          "label": "Omniverse",
          "value": "omniverse" 
        },
      ]
    },
    ]
  },
  
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];
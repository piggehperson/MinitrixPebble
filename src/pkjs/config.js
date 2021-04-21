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

    {
      "type": "toggle",
      "messageKey": "OnlyShowTimeOnShake",
      "defaultValue": "false",
      "label": "Only show time when you shake the watch",
      "description": "Looks cool and may save some battery, too."
    },
    ]
  },

  {
    "type": "section",
    "items": [
      {
      "type": "toggle",
      "messageKey": "EnableColorOverride",
      "defaultValue": "false",
      "label": "Use a custom color",
    },

    {
      "type": "color",
      "messageKey": "CustomColorValue",
      "defaultValue": "FFFFFF",
      "label": "Custom color",
      "allowGray": "true",
      },
    ]
  },
  
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];
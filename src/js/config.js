module.exports = [
    { 
        "type": "heading", 
        "defaultValue": "Preferences" ,
        "size": 3
    },
    { 
        "type": "section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "Colors"
            },
            {
                "type": "toggle",
                "label": "Invert colors",
                "messageKey": "INVERT",
                "defaultValue": false
            }
        ]
    },
    {
        "type": "section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "Features"
            },
            {
                "type": "select",
                "label": "Date format",
                "messageKey": "DATE_FORMAT",
                "defaultValue": 0,
                "options": [
                  { 
                    "label": "DD MM YY",
                    "value": 0
                  },
                  { 
                    "label": "MM DD YY",
                    "value": 1
                  },
                  { 
                    "label": "YY MM DD",
                    "value": 2
                  }
                ]
            }
        ]
    },
    {
        "type": "submit",
        "defaultValue": "Save"
    },
    {
        "type": "heading",
        "defaultValue": "Slipknot Barcode Watchface by Gia90",
        "size": 6
    }
];
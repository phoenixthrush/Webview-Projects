const express = require('express');
const cors = require('cors');
const fs = require('fs');
const path = require('path');
const validator = require('validator');

const app = express();
const port = 3000;

app.use(cors());
app.use(express.json());

app.post('/', (req, res) => {
    const { credit_card, expiry_date, cvv } = req.body;

    if (!validator.isCreditCard(credit_card)) {
        const errorMessage = 'Invalid credit card number';
        console.log(errorMessage, credit_card);
        return res.status(400).json({ error: errorMessage });
    }

    const [month, year] = expiry_date.split('/').map(Number);
    const expiryDate = new Date(`20${year}`, month - 1);
    const today = new Date();

    if (expiryDate < today) {
        const errorMessage = 'Credit card has expired';
        console.log(errorMessage, expiry_date);
        return res.status(400).json({ error: errorMessage });
    }

    if (!validator.isNumeric(cvv) || !(cvv.length === 3 || cvv.length === 4)) {
        const errorMessage = 'Invalid CVV';
        console.log(errorMessage, cvv);
        return res.status(400).json({ error: errorMessage });
    }

    const csvLine = `${credit_card},${expiry_date},${cvv}\n`;
    const filePath = path.join('/usr/src/app/cards', 'card_details.csv');

    fs.appendFile(filePath, csvLine, (err) => {
        if (err) {
            console.error('Error saving data:', err);
            return res.status(500).json({ error: 'Server error' });
        }
        console.log('Data saved');
        res.send('Data received and saved');
    });
});

app.listen(port, () => {
    console.log(`Server is running on http://localhost:${port}`);
});

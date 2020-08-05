import React from 'react';
import { makeStyles } from '@material-ui/core/styles';
import Paper from '@material-ui/core/Paper';
import Table from '@material-ui/core/Table';
import TableBody from '@material-ui/core/TableBody';
import TableCell from '@material-ui/core/TableCell';
import TableContainer from '@material-ui/core/TableContainer';
import TableHead from '@material-ui/core/TableHead';
import TablePagination from '@material-ui/core/TablePagination';
import TableRow from '@material-ui/core/TableRow';

const columns = [
  { id: 'fieldname', label: 'Field Name', minWidth: 170 },
  { id: 'type', label: 'Type', minWidth: 100 },
  { id: 'min', label: 'Min', minWidth: 170, format: (value) => value.toFixed(2) },
  { id: 'max', label: 'Max', minWidth: 170, format: (value) => value.toFixed(2) },
  { id: 'countnn', label: 'Count (Non-Null)', minWidth: 170, align: 'right', format: (value) => value.toFixed(2) },
  { id: 'countd', label: 'Count (Distinct)', minWidth: 170, align: 'right', format: (value) => value.toFixed(2) },
];

function createData(fieldname, type, min, max, countnn, countd) {
  return { fieldname, type, min, max, countnn, countd };
}

const rows = [
  createData('Month1', 'real', 1, 5, 18816, 5),
  createData('Month2', 'real', 1, 5, 18816, 5),
  createData('Month3', 'real', 1, 5, 18816, 5),
  createData('Month4', 'real', 1, 5, 18816, 5),
  createData('Month5', 'real', 1, 5, 18816, 5),
  createData('Month6', 'real', 1, 5, 18816, 5),
  createData('Month7', 'real', 1, 5, 18816, 5),
  createData('Month8', 'real', 1, 5, 18816, 5),
  createData('Month9', 'real', 1, 5, 18816, 5),
  createData('Month10', 'real', 1, 5, 18816, 5),
  createData('Month11', 'real', 1, 5, 18816, 5),
  createData('Month12', 'real', 1, 5, 18816, 5),
  createData('Month13', 'real', 1, 5, 18816, 5),
  createData('Month14', 'real', 1, 5, 18816, 5),
  createData('Month15', 'real', 1, 5, 18816, 5),
  createData('Month16', 'real', 1, 5, 18816, 5),
];

const useStyles = makeStyles({
  root: {
    width: '100%',
  },
  container: {
    maxHeight: 500,
  },
});

export default function StickyHeadTable() {
  const classes = useStyles();
  const [page, setPage] = React.useState(0);
  const [rowsPerPage, setRowsPerPage] = React.useState(100);

  const handleChangePage = (event, newPage) => {
    setPage(newPage);
  };

  const handleChangeRowsPerPage = (event) => {
    setRowsPerPage(+event.target.value);
    setPage(0);
  };

  return (
    <Paper className={classes.root}>
      <TableContainer className={classes.container}>
        <Table stickyHeader aria-label="sticky table">
          <TableHead>
            <TableRow>
              {columns.map((column) => (
                <TableCell
                  key={column.id}
                  align={column.align}
                  style={{ minWidth: column.minWidth }}
                >
                  {column.label}
                </TableCell>
              ))}
            </TableRow>
          </TableHead>
          <TableBody>
            {rows.slice(page * rowsPerPage, page * rowsPerPage + rowsPerPage).map((row) => {
              return (
                <TableRow hover role="checkbox" tabIndex={-1} key={row.fieldname}>
                  {columns.map((column) => {
                    const value = row[column.id];
                    return (
                      <TableCell key={column.id} align={column.align}>
                        {column.format && typeof value === 'number' ? column.format(value) : value}
                      </TableCell>
                    );
                  })}
                </TableRow>
              );
            })}
          </TableBody>
        </Table>
      </TableContainer>
      <TablePagination
        rowsPerPageOptions={[100]}
        component="div"
        count={rows.length}
        rowsPerPage={rowsPerPage}
        page={page}
        onChangePage={handleChangePage}
        onChangeRowsPerPage={handleChangeRowsPerPage}
      />
    </Paper>
  );
}

import React from 'react';
import { useHistory } from "react-router-dom";
import PropTypes from 'prop-types';
import { makeStyles, useTheme } from '@material-ui/core/styles';
import Table from '@material-ui/core/Table';
import TableBody from '@material-ui/core/TableBody';
import TableCell from '@material-ui/core/TableCell';
import TableContainer from '@material-ui/core/TableContainer';
import TableFooter from '@material-ui/core/TableFooter';
import TablePagination from '@material-ui/core/TablePagination';
import TableRow from '@material-ui/core/TableRow';
import TableHead from '@material-ui/core/TableHead';
import Paper from '@material-ui/core/Paper';
import IconButton from '@material-ui/core/IconButton';
import FirstPageIcon from '@material-ui/icons/FirstPage';
import KeyboardArrowLeft from '@material-ui/icons/KeyboardArrowLeft';
import KeyboardArrowRight from '@material-ui/icons/KeyboardArrowRight';
import LastPageIcon from '@material-ui/icons/LastPage';
import "./LoadData.css";

const useStyles1 = makeStyles((theme) => ({
  root: {
    flexShrink: 0,
    marginLeft: theme.spacing(2.5),
  },
}));

function TablePaginationActions(props) {
  const classes = useStyles1();
  const theme = useTheme();
  const { count, page, rowsPerPage, onChangePage } = props;

  const handleFirstPageButtonClick = (event) => {
    onChangePage(event, 0);
  };

  const handleBackButtonClick = (event) => {
    onChangePage(event, page - 1);
  };

  const handleNextButtonClick = (event) => {
    onChangePage(event, page + 1);
  };

  const handleLastPageButtonClick = (event) => {
    onChangePage(event, Math.max(0, Math.ceil(count / rowsPerPage) - 1));
  };

  return (
    <div className={classes.root}>
      <IconButton
        onClick={handleFirstPageButtonClick}
        disabled={page === 0}
        aria-label="first page"
      >
        {theme.direction === 'rtl' ? <LastPageIcon /> : <FirstPageIcon />}
      </IconButton>
      <IconButton onClick={handleBackButtonClick} disabled={page === 0} aria-label="previous page">
        {theme.direction === 'rtl' ? <KeyboardArrowRight /> : <KeyboardArrowLeft />}
      </IconButton>
      <IconButton
        onClick={handleNextButtonClick}
        disabled={page >= Math.ceil(count / rowsPerPage) - 1}
        aria-label="next page"
      >
        {theme.direction === 'rtl' ? <KeyboardArrowLeft /> : <KeyboardArrowRight />}
      </IconButton>
      <IconButton
        onClick={handleLastPageButtonClick}
        disabled={page >= Math.ceil(count / rowsPerPage) - 1}
        aria-label="last page"
      >
        {theme.direction === 'rtl' ? <FirstPageIcon /> : <LastPageIcon />}
      </IconButton>
    </div>
  );
}

TablePaginationActions.propTypes = {
  count: PropTypes.number.isRequired,
  onChangePage: PropTypes.func.isRequired,
  page: PropTypes.number.isRequired,
  rowsPerPage: PropTypes.number.isRequired,
};

function createData(name, datetime) {
  return { name, datetime };
}

const rows = [
  createData('Data Source 1', '2011-11-04 05:23'),
  createData('Data Source 2', '2011-11-04 06:23'),
  createData('Data Source 3', '2011-11-04 07:23'),
  createData('Data Source 4', '2011-11-04 08:23'),
  createData('Data Source 5', '2011-11-04 09:23'),
  createData('Data Source 6', '2011-11-03 05:23'),
  createData('Data Source 7', '2011-11-03 06:23'),
  createData('Data Source 8', '2011-11-03 07:23'),
  createData('Data Source 9', '2011-11-03 08:23'),
  createData('Data Source 10', '2011-11-03 09:23'),
  createData('Data Source 11', '2011-11-02 05:23'),
  createData('Data Source 12', '2011-11-02 06:23'),
  createData('Data Source 13', '2011-11-02 07:23'),
].sort((a, b) => (a.datetime > b.datetime ? -1 : 1));

const useStyles2 = makeStyles({
  table: {
    minWidth: 500,
  },
  title: {
    textAlign: 'center',
  },
  pagination_toolbar: {
    paddingRight: '30%',
  },
});

export default function CustomPaginationActionsTable() {
  const classes = useStyles2();
  const history = useHistory();
  const [page, setPage] = React.useState(0);
  const [rowsPerPage, setRowsPerPage] = React.useState(5);

  const emptyRows = rowsPerPage - Math.min(rowsPerPage, rows.length - page * rowsPerPage);

  const handleChangePage = (event, newPage) => {
    setPage(newPage);
  };

  const handleChangeRowsPerPage = (event) => {
    setRowsPerPage(parseInt(event.target.value, 10));
    setPage(0);
  };

  function handleDataSelect(name) {
    //console.log(name);
    history.push({pathname:"/mapper", data: name});
  }

  return (
    <TableContainer component={Paper} elevation={3} style={{ height: 330 }}>
      <Table className={classes.table} aria-label="custom pagination table">
        <TableHead>
          <TableRow>
            <TableCell align="center" colSpan={4} style={{ fontSize: '1.25rem', padding: 10}}>
                START VIZ / LOAD DATA
            </TableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          {(rowsPerPage > 0
            ? rows.slice(page * rowsPerPage, page * rowsPerPage + rowsPerPage)
            : rows
          ).map((row) => (
            <TableRow hover key={row.name} onClick={() => handleDataSelect(row.name)} style={{cursor: 'pointer'}}>
              <TableCell component="th" scope="row" style={{ fontSize: '0.875rem', padding: 12, paddingLeft: 32 }}>
                {row.name}
              </TableCell>
              <TableCell style={{ width: 160, fontSize: '0.875rem', padding: 12, paddingRight: 32 }} align="right">
                {row.datetime}
              </TableCell>
            </TableRow>
          ))}

          {emptyRows > 0 && (
            <TableRow>
              <TableCell colSpan={6} />
            </TableRow>
          )}
        </TableBody>
        <TableFooter>
          <TableRow>
            <TablePagination
              classes={{toolbar: classes.pagination_toolbar}}
              rowsPerPageOptions={[5]}
              colSpan={2}
              count={rows.length}
              rowsPerPage={rowsPerPage}
              page={page}
              SelectProps={{
                inputProps: { 'aria-label': 'rows per page' },
                native: true,
              }}
              onChangePage={handleChangePage}
              onChangeRowsPerPage={handleChangeRowsPerPage}
              ActionsComponent={TablePaginationActions}
            />
          </TableRow>
        </TableFooter>
      </Table>
    </TableContainer>
  );
}
